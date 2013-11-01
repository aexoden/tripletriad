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

#include <iostream>
#include <sstream>
#include <string>

#include "board.hh"
#include "common.hh"

std::vector<std::string> get_input()
{
	std::string input;

	std::cout << ">>> ";
	std::getline(std::cin, input);

	std::vector<std::string> tokens;

	size_t current = 0;
	size_t next = input.find_first_of(' ', current);

	while (next != std::string::npos)
	{
		std::string token = input.substr(current, next - current);

		current = next + 1;
		next = input.find_first_of(' ', current);

		if (!token.empty())
			tokens.push_back(token);
	}

	std::string token = input.substr(current, next - current);

	if (!token.empty())
		tokens.push_back(token);

	return tokens;
}

int main(int argc, char ** argv)
{
	std::shared_ptr<Board> board;
	std::vector<bool> human(2);

	bool run = true;
	bool started = false;

	while (run)
	{
		if (started)
		{
			std::cout << "SCORE:    Red: " << (board->get_score(PLAYER_RED)) << "   Blue: " << (board->get_score(PLAYER_BLUE)) << std::endl;

			if (human[board->get_current_player()])
			{
				auto tokens = get_input();

				if (tokens.empty())
					continue;

				if (tokens[0] == "play")
				{
					int row;
					int column;

					std::istringstream row_stream(tokens[1]);
					row_stream >> row;

					std::istringstream column_stream(tokens[2]);
					column_stream >> column;

					std::string name = "";

					for (size_t i = 3; i < tokens.size(); i++)
					{
						if (i > 3)
							name += " ";

						name += tokens[i];
					}

					auto move = board->get_move(row - 1, column - 1, name);

					if (!board->move(move, true))
						std::cout << "Invalid move, Captain. Try again." << std::endl;
				}
				else if (tokens[0] == "exit")
				{
					run = false;
				}
			}
			else
			{
				auto move = board->suggest_move();

				board->move(move, true);
			}
		}
		else
		{
			auto tokens = get_input();

			if (tokens.empty())
				continue;

			if (tokens[0] == "new")
			{
				Player first_player = tokens[1] == "blue" ? PLAYER_BLUE : PLAYER_RED;

				bool elemental = false;

				for (size_t i = 2; i < tokens.size(); i++)
				{
					if (tokens[i] == "elemental")
						elemental = true;
				}

				board = std::make_shared<Board>(first_player, elemental);
				started = false;
			}
			else if (tokens[0] == "element")
			{
				int row;
				int column;

				std::istringstream row_stream(tokens[1]);
				row_stream >> row;

				std::istringstream column_stream(tokens[2]);
				column_stream >> column;

				Element element = ELEMENT_NONE;

				if (tokens[3] == "fire")
					element = ELEMENT_FIRE;
				else if (tokens[3] == "ice")
					element = ELEMENT_ICE;
				else if (tokens[3] == "thunder")
					element = ELEMENT_THUNDER;
				else if (tokens[3] == "poison")
					element = ELEMENT_POISON;
				else if (tokens[3] == "earth")
					element = ELEMENT_EARTH;
				else if (tokens[3] == "wind")
					element = ELEMENT_WIND;
				else if (tokens[3] == "water")
					element = ELEMENT_WATER;
				else if (tokens[3] == "holy")
					element = ELEMENT_HOLY;

				board->set_element(row - 1, column - 1, element);
			}
			else if (tokens[0] == "human")
			{
				Player player = tokens[1] == "blue" ? PLAYER_BLUE : PLAYER_RED;
				human[player] = true;
			}
			else if (tokens[0] == "card")
			{
				Player player = tokens[1] == "blue" ? PLAYER_BLUE : PLAYER_RED;

				std::string name = "";

				for (size_t i = 2; i < tokens.size(); i++)
				{
					if (i > 2)
						name += " ";

					name += tokens[i];
				}

				if (!board->activate_card(player, name))
					std::cout << "WARNING:  Invalid card" << std::endl;
			}
			else if (tokens[0] == "start")
			{
				started = true;
			}
			else if (tokens[0] == "exit")
			{
				run = false;
			}
		}

		if (board && board->is_complete())
			run = false;
	}

	if (board)
		std::cout << "SCORE:    Red: " << board->get_score(PLAYER_RED) << "   Blue: " << board->get_score(PLAYER_BLUE) << std::endl;

	return 0;
}
