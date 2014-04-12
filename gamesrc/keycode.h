/*
Minetest
Copyright (C) 2010-2013 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef KEYCODE_HEADER
#define KEYCODE_HEADER

#include "irrlichttypes.h"
#include <IEventReceiver.h>
#include <string>
#include <list>
/* A key press, consisting of either an Irrlicht keycode
   or an actual char */

class KeyPress
{
public:
	KeyPress();
	KeyPress(const char *name);

	KeyPress(const irr::SEvent::SKeyInput &in, bool prefer_character=false);

	bool operator==(const KeyPress &o) const
	{
		return (Char > 0 && Char == o.Char) ||
			(valid_kcode(Key) && Key == o.Key);
	}

	const char *sym() const;
	const char *name() const;

	std::string debug() const;
protected:
	static bool valid_kcode(irr::EKEY_CODE k)
	{
		return k > 0 && k < irr::KEY_KEY_CODES_COUNT;
	}

	irr::EKEY_CODE Key;
	wchar_t Char;
	std::string m_name;
};

extern const KeyPress EscapeKey;
extern const KeyPress NumberKey[10];

// Key configuration getter
KeyPress getKeySetting(const char *settingname);

// Clear fast lookup cache
void clearKeyCache();

class KeyList : protected std::list<KeyPress>
{
	typedef std::list<KeyPress> super;
	typedef super::iterator iterator;
	typedef super::const_iterator const_iterator;
    
	virtual const_iterator find(const KeyPress &key) const
	{
		const_iterator f(begin());
		const_iterator e(end());
		while (f!=e) {
			if (*f == key)
				return f;
			++f;
		}
		return e;
	}
    
	virtual iterator find(const KeyPress &key)
	{
		iterator f(begin());
		iterator e(end());
		while (f!=e) {
			if (*f == key)
				return f;
			++f;
		}
		return e;
	}
    
public:
	void clear() { super::clear(); }
    
	void set(const KeyPress &key)
	{
		if (find(key) == end())
			push_back(key);
	}
    
	void unset(const KeyPress &key)
	{
		iterator p(find(key));
		if (p != end())
			erase(p);
	}
    
	void toggle(const KeyPress &key)
	{
		iterator p(this->find(key));
		if (p != end())
			erase(p);
		else
			push_back(key);
	}
    
	bool operator[](const KeyPress &key) const
	{
		return find(key) != end();
	}
};

#endif

