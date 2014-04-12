/*
Minetest
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>

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

#ifndef GETTEXT_HEADER
#define GETTEXT_HEADER

#include "config.h" // for USE_GETTEXT
#include "log.h"
#include "util/string.h"
#if USE_GETTEXT
#include <libintl.h>
#else
#define gettext(String) String
#endif

#define _(String) chartowchar_t(String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>

#endif // #if defined(_WIN32)

#ifdef _MSC_VER
void init_gettext(const char *path,std::string configured_language,int argc, char** argv);
#else
void init_gettext(const char *path,std::string configured_language);
#endif

/******************************************************************************/
inline wchar_t* chartowchar_t(const char *str)
{
    return char_to_wide(str);
}

/******************************************************************************/
wchar_t* wgettext(const char *str);

/******************************************************************************/
std::wstring wstrgettext(std::string text);

#endif
