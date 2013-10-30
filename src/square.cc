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

#include <sstream>

#include "square.hh"

Square::Square(int row, int column) :
	row(row),
	column(column),
	_neighbors(4)
{ }

const std::shared_ptr<Square> & Square::get_neighbor(Direction direction) const
{
	return this->_neighbors[direction];
}

std::vector<std::shared_ptr<Square>> Square::create_squares(int rows, int columns)
{
	std::vector<std::shared_ptr<Square>> squares(rows * columns);

	for (int row = 0; row < rows; row++)
		for (int column = 0; column < columns; column++)
			squares[row * columns + column] = std::make_shared<Square>(row, column);

	for (int row = 0; row < rows; row++)
	{
		for (int column = 0; column < columns; column++)
		{
			if (column > 0)
				squares[row * columns + column]->_neighbors[WEST] = squares[row * columns + column - 1];

			if (row > 0)
				squares[row * columns + column]->_neighbors[NORTH] = squares[(row - 1) * columns + column];

			if (column < columns - 1)
				squares[row * columns + column]->_neighbors[EAST] = squares[row * columns + column + 1];

			if (row < rows - 1)
				squares[row * columns + column]->_neighbors[SOUTH] = squares[(row + 1) * columns + column];
		}
	}

	return squares;
}

std::ostream & operator<<(std::ostream & stream, const Square & square)
{
	std::ostringstream oss;
	oss << "(" << square.row + 1 << ", " << square.column + 1 << ")";

	return stream << oss.str();
}
