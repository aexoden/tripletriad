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

Board::Board(Player first_player) :
	_current_player(first_player),
	_unplayed_cards(2),
	_unplayed_card_counts(2, 5),
	_squares(Square::create_squares(3, 3))
{
	_initialize_cards();
	_initialize_moves();
}

void Board::activate_card(Player player, const std::string & name)
{
	_unplayed_cards[player][_cards[name]]++;
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

bool Board::move(const std::shared_ptr<Move> & move)
{
	if (move->square->card)
		return false;

	if (_unplayed_cards[_current_player][move->card] == 0)
		return false;

	_move(move);

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

					_move(move);
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

void Board::_move(const std::shared_ptr<Move> & move)
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
		int score = 0;

		switch(direction)
		{
			case NORTH: score = source->card->top - target->card->bottom; break;
			case SOUTH: score = source->card->bottom - target->card->top; break;
			case WEST: score = source->card->left - target->card->right; break;
			case EAST: score = source->card->right - target->card->left; break;
		}

		if (score > 0)
		{
			target->owner = source->owner;
			_flip_history.push(target);
		}
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

					_move(move);
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
