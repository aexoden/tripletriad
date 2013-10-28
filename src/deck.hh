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

#ifndef TRIPLETRIAD_DECK_HH
#define TRIPLETRIAD_DECK_HH

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "card.hh"

class Deck
{
	public:
		Deck();

		void add_card(const std::string & name);
		std::shared_ptr<Card> remove_card(const std::string & name);

		void add_level(int level);

		std::unordered_set<std::string> get_valid_card_names();
		int get_remaining_cards();

	private:
		void _initialize_card(const std::shared_ptr<Card> & card);
		void _initialize_cards();

		std::unordered_map<std::string, std::shared_ptr<Card>> _cards;
		std::unordered_map<std::string, int> _active_cards;

		int _remaining_cards;
};

#endif
