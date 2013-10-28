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
#include <iostream>
#include <limits>

#include "auto_player.hh"

AutoPlayer::AutoPlayer(const std::shared_ptr<Board> & board) :
	_board(board)
{ }

std::shared_ptr<Move> AutoPlayer::get_next_move()
{
	Player self = _board->get_current_player();

	int positions = 0;
	int best_score = std::numeric_limits<int>::min();

	std::shared_ptr<Move> best_move;

	auto moves = _board->get_valid_moves();

	for (auto & move : moves)
	{
		_board->move(move);

		std::cout << "Evaluating " << *move << std::endl;

		int score = _search_minimax(self, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), positions);

		_board->unmove();

		if (score > best_score)
		{
			best_score = score;
			best_move = move;
		}

		positions++;
	}

	std::cout << std::left << "COMPUTER: ";
	std::cout << std::setw(11) << "Positions:" << std::setw(12) << positions;
	std::cout << std::setw(6) << "Move:" << std::setw(30) << *best_move;
	std::cout << std::setw(10) << "Utility:" << std::setw(10) << best_score;
	std::cout << std::endl;

	return best_move;
}

int AutoPlayer::_search_minimax(Player self, int alpha, int beta, int & positions)
{
	auto moves = _board->get_valid_moves();

	if (moves.empty())
		return _evaluate(self);

	for (auto & move : moves)
	{
		_board->move(move);

		int score = _search_minimax(self, alpha, beta, positions);

		_board->unmove();

		if (_board->get_current_player() == self)
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

	if (_board->get_current_player() == self)
		return alpha;
	else
		return beta;
}

int AutoPlayer::_evaluate(Player player)
{
	return _board->get_score(player) - _board->get_score(player == PLAYER_RED ? PLAYER_BLUE : PLAYER_RED);
}
