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

#include "deck.hh"

Deck::Deck() :
	_remaining_cards(5)
{
	_initialize_cards();
}

void Deck::add_card(const std::string & name)
{
	if (_active_cards[name] < 5)
		_active_cards[name]++;

	if (_remaining_cards < 5)
		_remaining_cards++;
}

std::shared_ptr<Card> Deck::remove_card(const std::string & name)
{
	if (_active_cards[name] > 0)
	{
		_active_cards[name]--;
		_remaining_cards--;
		return _cards[name];
	}
	else
	{
		return nullptr;
	}
}

void Deck::add_level(int level)
{
	for (auto & pair : _cards)
	{
		if (pair.second->level == level)
		{
			_active_cards[pair.second->name] = 5;
		}
	}
}

std::unordered_set<std::string> Deck::get_valid_card_names()
{
	std::unordered_set<std::string> card_names;

	for (auto & pair : _active_cards)
	{
		if (pair.second > 0)
		{
			card_names.insert(pair.first);
		}
	}

	return card_names;
}

int Deck::get_remaining_cards()
{
	return _remaining_cards;
}

void Deck::_initialize_card(const std::shared_ptr<Card> & card)
{
	_cards.insert(std::make_pair(card->name, card));
}

void Deck::_initialize_cards()
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
