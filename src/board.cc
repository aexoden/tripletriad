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

#include "board.hh"

Board::Board(Player first_player) :
	_current_player(first_player),
	_unplayed_cards(2),
	_unplayed_card_counts(2),
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

void Board::move(const std::shared_ptr<Move> & move)
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

void Board::unmove()
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

std::vector<std::shared_ptr<Move>> Board::get_valid_moves() const
{
	std::vector<std::shared_ptr<Move>> moves;
	moves.reserve(16);

	for (auto & square : _squares)
	{
		if (!square->card)
		{
			for (auto & pair : _unplayed_cards[_current_player])
			{
				if (pair.second > 0)
				{
					moves.push_back(square->moves.at(pair.first));
				}
			}
		}
	}

	return moves;
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
