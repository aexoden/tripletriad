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

#ifndef TRIPLETRIAD_SQUARE_HH
#define TRIPLETRIAD_SQUARE_HH

#include <memory>
#include <unordered_map>
#include <vector>

#include "card.hh"
#include "common.hh"

class Move;

class Square
{
	public:
		Square(int row, int column);

		std::shared_ptr<Square> get_neighbor(Direction direction) const;

		static std::vector<std::shared_ptr<Square>> create_squares(int rows, int columns);

		friend std::ostream & operator<<(std::ostream & stream, const Square & square);

		const int row;
		const int column;

		Element element;

		Player owner;

		std::shared_ptr<Card> card;

		std::unordered_map<std::shared_ptr<Card>, std::shared_ptr<Move>> moves;

	private:
		std::vector<std::shared_ptr<Square>> _neighbors;
};

#endif
