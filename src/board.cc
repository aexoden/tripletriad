/*
 * Copyright (c) 2013 Jason Lynch <jason@calindora.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <iomanip>
#include <limits>

#include "board.hh"

Board::Board(Player first_player, bool elemental) :
	_current_player(first_player),
	_elemental(elemental),
	_unplayed_cards(2),
	_unplayed_card_counts(2, 5),
	_squares(Square::create_squares(3, 3))
{
	_initialize_cards();
	_initialize_moves();
}

bool Board::activate_card(Player player, const std::string & name)
{
	if (_cards.count(name))
	{
		_unplayed_cards[player][_cards[name]]++;
		return true;
	}
	else
	{
		return false;
	}
}

void Board::activate_card_level(Player player, int level)
{
	for (auto & pair : _cards)
	{
		if (pair.second->level == level)
		{
			_unplayed_cards[player][pair.second] = 5;
		}
	}
}

void Board::set_element(int row, int column, Element element)
{
	auto square = _squares[row * 3 + column];

	square->element = element;
}

bool Board::move(const std::shared_ptr<Move> & move, bool output)
{
	if (move->square->card)
		return false;

	if (_unplayed_cards[_current_player][move->card] == 0)
		return false;

	_move(move, output);

	if (output)
	{
		std::cout << "MOVE:     " << (_current_player == PLAYER_RED ? "Red" : "Blue") << " plays " << *move;

		int elemental_adjustment = _get_elemental_adjustment(move->square);

		if (elemental_adjustment == 0)
			std::cout << std::endl;
		else
			std::cout << " (Elemental bonus: " << elemental_adjustment << ")" << std::endl;
	}

	return true;
}

Player Board::get_current_player() const
{
	return _current_player;
}

int Board::get_score(Player player) const
{
	int score = _unplayed_card_counts[player];

	for (auto & square : _squares)
	{
		if (square->card && square->owner == player)
		{
			score++;
		}
	}

	return score;
}

bool Board::is_complete() const
{
	for (auto & square : _squares)
	{
		if (!square->card)
			return false;
	}

	return true;
}

std::shared_ptr<Move> Board::get_move(int row, int column, const std::string & name)
{
	auto square = _squares[row * 3 + column];
	return square->moves[_cards[name]];
}

std::shared_ptr<Move> Board::suggest_move()
{
	Player self(_current_player);

	int positions = 0;
	int best_score = std::numeric_limits<int>::min();

	std::shared_ptr<Move> best_move;

	for (auto & square : _squares)
	{
		if (!square->card)
		{
			for (auto & pair : _unplayed_cards[_current_player])
			{
				if (pair.second > 0)
				{
					std::shared_ptr<Move> move(square->moves.at(pair.first));

					_move(move, false);
					int score = _search_minimax(self, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), positions);
					_unmove();

					if (score > best_score)
					{
						best_score = score;
						best_move = move;
					}

					positions++;
				}
			}
		}
	}

	std::cout << std::left << "COMPUTER: ";
	std::cout << std::setw(11) << "Positions:" << std::setw(12) << positions;
	std::cout << std::setw(6) << "Move:" << std::setw(30) << *best_move;
	std::cout << std::setw(10) << "Utility:" << std::setw(10) << best_score;
	std::cout << std::endl;

	return best_move;
}

void Board::_move(const std::shared_ptr<Move> & move, bool output)
{
	_move_history.push(move);
	_flip_history.push(move->square);

	_unplayed_cards[_current_player][move->card]--;
	_unplayed_card_counts[_current_player]--;

	move->square->card = move->card;
	move->square->owner = _current_player;

	_execute_basic(move->square, NORTH);
	_execute_basic(move->square, SOUTH);
	_execute_basic(move->square, WEST);
	_execute_basic(move->square, EAST);

	_change_player();
}

void Board::_unmove()
{
	auto move = _move_history.top();
	_move_history.pop();

	_change_player();

	for (auto square = _flip_history.top(); square != move->square; square = _flip_history.top())
	{
		square->owner = square->owner == PLAYER_RED ? PLAYER_BLUE : PLAYER_RED;
		_flip_history.pop();
	}

	_flip_history.pop();

	_unplayed_cards[_current_player][move->card]++;
	_unplayed_card_counts[_current_player]++;
	move->square->card = nullptr;
}

void Board::_change_player()
{
	_current_player = _current_player == PLAYER_RED ? PLAYER_BLUE : PLAYER_RED;
}

void Board::_execute_basic(const std::shared_ptr<Square> & source, Direction direction)
{
	auto target = source->get_neighbor(direction);

	if (!target || !target->card)
		return;

	if (target->owner != source->owner)
	{
		int score = _get_elemental_adjustment(source) - _get_elemental_adjustment(target);

		switch(direction)
		{
			case NORTH:
				score += source->card->top;
				score -= target->card->bottom;
				break;

			case SOUTH:
				score += source->card->bottom;
				score -= target->card->top;
				break;

			case WEST:
				score += source->card->left;
				score -= target->card->right;
				break;

			case EAST:
				score += source->card->right;
				score -= target->card->left;
				break;
		}

		if (score > 0)
		{
			target->owner = source->owner;
			_flip_history.push(target);
		}
	}
}

int Board::_get_elemental_adjustment(const std::shared_ptr<Square> & square)
{
	if (_elemental && square->element != ELEMENT_NONE)
	{
		if (square->element == square->card->element)
			return 1;
		else
			return -1;
	}
	else
	{
		return 0;
	}
}

void Board::_initialize_card(const std::shared_ptr<Card> & card)
{
	_cards.insert(std::make_pair(card->name, card));
}

void Board::_initialize_cards()
{
	_initialize_card(std::make_shared<Card>(1, "Geezard", 1, 1, 5, 4, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(1, "Funguar", 5, 1, 3, 1, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(1, "Bite Bug", 1, 3, 5, 3, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(1, "Red Bat", 6, 1, 2, 1, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(1, "Blobra", 2, 1, 5, 3, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(1, "Gayla", 2, 4, 4, 1, ELEMENT_THUNDER));
	_initialize_card(std::make_shared<Card>(1, "Gesper", 1, 4, 1, 5, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(1, "Fastitocalon-F", 3, 2, 1, 5, ELEMENT_EARTH));
	_initialize_card(std::make_shared<Card>(1, "Blood Soul", 2, 6, 1, 1, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(1, "Caterchipillar", 4, 4, 3, 2, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(1, "Cockatrice", 2, 2, 6, 1, ELEMENT_THUNDER));

	_initialize_card(std::make_shared<Card>(2, "Grat", 7, 3, 1, 1, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(2, "Buel", 6, 2, 3, 2, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(2, "Mesmerize", 5, 3, 4, 3, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(2, "Glacial Eye", 6, 4, 3, 1, ELEMENT_ICE));
	_initialize_card(std::make_shared<Card>(2, "Belhelmel", 3, 5, 3, 4, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(2, "Thrustaevis", 5, 2, 5, 3, ELEMENT_WIND));
	_initialize_card(std::make_shared<Card>(2, "Anacondaur", 5, 3, 5, 1, ELEMENT_POISON));
	_initialize_card(std::make_shared<Card>(2, "Creeps", 5, 5, 2, 2, ELEMENT_THUNDER));
	_initialize_card(std::make_shared<Card>(2, "Grendel", 4, 5, 2, 4, ELEMENT_THUNDER));
	_initialize_card(std::make_shared<Card>(2, "Jelleye", 3, 1, 7, 2, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(2, "Grand Mantis", 5, 5, 3, 2, ELEMENT_NONE));

	_initialize_card(std::make_shared<Card>(3, "Forbidden", 6, 3, 2, 6, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(3, "Armadodo", 6, 1, 6, 3, ELEMENT_EARTH));
	_initialize_card(std::make_shared<Card>(3, "Tri-Face", 3, 5, 5, 5, ELEMENT_POISON));
	_initialize_card(std::make_shared<Card>(3, "Fastitocalon", 7, 1, 3, 5, ELEMENT_EARTH));
	_initialize_card(std::make_shared<Card>(3, "Snow Lion", 7, 5, 3, 1, ELEMENT_ICE));
	_initialize_card(std::make_shared<Card>(3, "Ochu", 5, 3, 3, 6, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(3, "SAM08G", 5, 2, 4, 6, ELEMENT_FIRE));
	_initialize_card(std::make_shared<Card>(3, "Death Claw", 4, 7, 2, 4, ELEMENT_FIRE));
	_initialize_card(std::make_shared<Card>(3, "Cactuar", 6, 6, 3, 2, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(3, "Tonberry", 3, 4, 4, 6, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(3, "Abyss Worm", 7, 3, 5, 2, ELEMENT_EARTH));

	_initialize_card(std::make_shared<Card>(4, "Turtapod", 2, 6, 7, 3, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(4, "Vysage", 6, 4, 5, 5, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(4, "T-Rexaur", 4, 2, 7, 6, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(4, "Bomb", 2, 6, 3, 7, ELEMENT_FIRE));
	_initialize_card(std::make_shared<Card>(4, "Blitz", 1, 4, 7, 6, ELEMENT_THUNDER));
	_initialize_card(std::make_shared<Card>(4, "Wendigo", 7, 1, 6, 3, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(4, "Torama", 7, 4, 4, 4, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(4, "Imp", 3, 3, 6, 7, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(4, "Blue Dragon", 6, 7, 3, 2, ELEMENT_POISON));
	_initialize_card(std::make_shared<Card>(4, "Adamantoise", 4, 5, 6, 5, ELEMENT_EARTH));
	_initialize_card(std::make_shared<Card>(4, "Hexadragon", 7, 4, 3, 5, ELEMENT_FIRE));

	_initialize_card(std::make_shared<Card>(5, "Iron Giant", 6, 6, 5, 5, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(5, "Behemoth", 3, 5, 7, 6, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(5, "Chimera", 7, 5, 3, 6, ELEMENT_WATER));
	_initialize_card(std::make_shared<Card>(5, "PuPu", 3, 2, 1, 10, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(5, "Elastoid", 6, 6, 7, 2, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(5, "GIM47N", 5, 7, 4, 5, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(5, "Malboro", 7, 4, 2, 7, ELEMENT_POISON));
	_initialize_card(std::make_shared<Card>(5, "Ruby Dragon", 7, 7, 4, 2, ELEMENT_FIRE));
	_initialize_card(std::make_shared<Card>(5, "Elnoyle", 5, 7, 6, 3, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(5, "Tonberry King", 4, 7, 4, 6, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(5, "Wedge, Biggs", 6, 2, 7, 6, ELEMENT_NONE));

	_initialize_card(std::make_shared<Card>(6, "Fujin, Raijin", 2, 8, 4, 8, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(6, "Elvoret", 7, 3, 4, 8, ELEMENT_WIND));
	_initialize_card(std::make_shared<Card>(6, "X-ATM092", 4, 7, 3, 8, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(6, "Granaldo", 7, 8, 5, 2, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(6, "Gerogero", 1, 8, 3, 8, ELEMENT_POISON));
	_initialize_card(std::make_shared<Card>(6, "Iguion", 8, 8, 2, 2, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(6, "Abadon", 6, 4, 5, 8, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(6, "Trauma", 4, 5, 6, 8, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(6, "Oilboyle", 1, 4, 8, 8, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(6, "Shumi Tribe", 6, 8, 4, 5, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(6, "Krysta", 7, 8, 1, 5, ELEMENT_NONE));

	_initialize_card(std::make_shared<Card>(7, "Propagator", 8, 4, 8, 4, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(7, "Jumbo Cactuar", 8, 4, 4, 8, ELEMENT_NONE));
	//_initialize_card(std::make_shared<Card>(7, "));
	_initialize_card(std::make_shared<Card>(7, "Gargantua", 5, 6, 8, 6, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(7, "Mobile Type 8", 8, 7, 3, 6, ELEMENT_NONE));
	//_initialize_card(std::make_shared<Card>());
	_initialize_card(std::make_shared<Card>(7, "Tiamat", 8, 5, 4, 8, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(7, "BGH251F2", 5, 8, 5, 7, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(7, "Red Giant", 6, 4, 7, 8, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(7, "Catoblepas", 1, 7, 7, 8, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(7, "Ultima Weapon", 7, 2, 8, 7, ELEMENT_NONE));

	_initialize_card(std::make_shared<Card>(8, "Chubby Chocobo", 4, 8, 9, 4, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(8, "Angelo", 9, 7, 3, 6, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(8, "Gilgamesh", 3, 9, 6, 7, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(8, "MiniMog", 9, 9, 2, 3, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(8, "Chicobo", 9, 8, 4, 4, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(8, "Quezacotl", 2, 9, 4, 9, ELEMENT_THUNDER));
	//_initialize_card(std::make_shared<Card>());
	_initialize_card(std::make_shared<Card>(8, "Ifrit", 9, 2, 8, 6, ELEMENT_FIRE));
	_initialize_card(std::make_shared<Card>(8, "Siren", 8, 6, 2, 9, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(8, "Sacred", 5, 9, 9, 1, ELEMENT_EARTH));
	_initialize_card(std::make_shared<Card>(8, "Minotaur", 9, 2, 9, 5, ELEMENT_EARTH));

	_initialize_card(std::make_shared<Card>(9, "Carbuncle", 8, 10, 4, 4, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(9, "Diablos", 5, 8, 3, 10, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(9, "Leviathan", 7, 1, 7, 10, ELEMENT_WATER));
	//_initialize_card(std::make_shared<Card>());
	_initialize_card(std::make_shared<Card>(9, "Pandemona", 10, 7, 7, 1, ELEMENT_WIND));
	_initialize_card(std::make_shared<Card>(9, "Cerberus", 7, 6, 10, 4, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(9, "Alexander", 9, 4, 2, 10, ELEMENT_HOLY));
	_initialize_card(std::make_shared<Card>(9, "Phoenix", 7, 7, 10, 2, ELEMENT_FIRE));
	_initialize_card(std::make_shared<Card>(9, "Bahamut", 10, 2, 6, 8, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(9, "Doomtrain", 3, 10, 10, 1, ELEMENT_POISON));
	//_initialize_card(std::make_shared<Card>());

	_initialize_card(std::make_shared<Card>(10, "Ward", 10, 2, 8, 7, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(10, "Kiros", 6, 6, 10, 7, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(10, "Laguna", 5, 3, 9, 10, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(10, "Selphie", 10, 6, 4, 8, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(10, "Quistis", 9, 10, 2, 6, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(10, "Irvine", 2, 9, 10, 6, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(10, "Zell", 8, 10, 6, 5, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(10, "Rinoa", 4, 2, 10, 10, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(10, "Edea", 10, 3, 3, 10, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(10, "Seifer", 6, 10, 4, 9, ELEMENT_NONE));
	_initialize_card(std::make_shared<Card>(10, "Squall", 10, 6, 9, 4, ELEMENT_NONE));
}

void Board::_initialize_moves()
{
	for (auto & square : _squares)
	{
		for (auto & pair : _cards)
		{
			square->moves.insert(std::make_pair(pair.second, std::make_shared<Move>(square, pair.second)));
		}
	}
}

int Board::_search_minimax(Player self, int alpha, int beta, int & positions)
{
	bool valid_move = false;

	for (auto & square : _squares)
	{
		if (!square->card)
		{
			for (auto & pair : _unplayed_cards[_current_player])
			{
				if (pair.second > 0)
				{
					valid_move = true;
					std::shared_ptr<Move> move(square->moves.at(pair.first));

					_move(move, false);
					int score = _search_minimax(self, alpha, beta, positions);
					_unmove();

					if (get_current_player() == self)
					{
						if (score >= beta)
							return beta;

						if (score > alpha)
							alpha = score;
					}
					else
					{
						if (score <= alpha)
							return alpha;

						if (score < beta)
							beta = score;
					}

					positions++;
				}
			}
		}
	}

	if (!valid_move)
		return _evaluate(self);
	else if (get_current_player() == self)
		return alpha;
	else
		return beta;
}

int Board::_evaluate(Player player)
{
	return get_score(player) - get_score(player == PLAYER_RED ? PLAYER_BLUE : PLAYER_RED);
}
