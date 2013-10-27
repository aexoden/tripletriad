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

#ifndef TRIPLETRIAD_BOARD_HH
#define TRIPLETRIAD_BOARD_HH

#include <memory>
#include <stack>
#include <vector>

#include "common.hh"
#include "deck.hh"
#include "move.hh"
#include "square.hh"

class Board
{
	public:
		Board(Player first_player, std::unique_ptr<Deck> red_deck, std::unique_ptr<Deck> blue_deck);

		void move(const std::shared_ptr<Move> & move);
		void unmove();

		std::unordered_set<std::shared_ptr<Move>> get_valid_moves() const;

	private:
		void _change_player();
		void _execute_basic(const std::shared_ptr<Square> & source, Direction direction);

		Player _current_player;

		std::vector<std::vector<std::shared_ptr<Square>>> _grid;
		std::vector<std::unique_ptr<Deck>> _decks;

		std::stack<std::shared_ptr<Move>> _move_history;
		std::stack<std::shared_ptr<Square>> _flip_history;
};

#endif
