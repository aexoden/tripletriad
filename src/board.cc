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

Board::Board(Player first_player, std::unique_ptr<Deck> red_deck, std::unique_ptr<Deck> blue_deck) :
	_current_player(first_player),
	_grid(Square::create_grid(3, 3)),
	_decks(2)
{
	_decks[PLAYER_RED] = std::move(red_deck);
	_decks[PLAYER_BLUE] = std::move(blue_deck);
}

void Board::move(const std::shared_ptr<Move> & move)
{
	_move_history.push(move);
	_flip_history.push(move->square);

	move->square->card = _decks[_current_player]->remove_card(move->card_name);
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

	_decks[_current_player]->add_card(move->card_name);
	move->square->card = nullptr;
}

Player Board::get_current_player() const
{
	return _current_player;
}

int Board::get_score(Player player) const
{
	int score = 0;

	for (auto & row : _grid)
	{
		for (auto & square : row)
		{
			if (square->card && square->owner == player)
			{
				score++;
			}
		}
	}

	return score;
}

std::unordered_set<std::shared_ptr<Move>> Board::get_valid_moves() const
{
	std::unordered_set<std::shared_ptr<Move>> moves;
	std::unordered_set<std::string> card_names = _decks[_current_player]->get_valid_card_names();

	for (size_t row = 0; row < _grid.size(); row++)
	{
		for (size_t column = 0; column < _grid[row].size(); column++)
		{
			std::shared_ptr<Square> square = _grid[row][column];

			if (!square->card)
			{
				for (auto iter = card_names.begin(); iter != card_names.end(); iter++)
				{
					moves.insert(std::make_shared<Move>(square, *iter));
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
