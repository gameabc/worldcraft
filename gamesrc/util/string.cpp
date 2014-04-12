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

#include "string.h"
#include "pointer.h"
#include "numeric.h"

#include "../sha1.h"
#include "../base64.h"
#include "../porting.h"
#include "gettext.h"

#ifdef ANDROID
int wctomb(char *s, wchar_t wc) { return wcrtomb(s,wc,NULL); }
int mbtowc(wchar_t *pwc, const char *s, size_t n) { return mbrtowc(pwc, s, n, NULL); }
#endif
#include "utf8/utf8.h"

char* wide_to_char(const wchar_t* input)
{
    static std::vector<char> utf8line;
    utf8line.clear();
    
    utf8::utf16to8(input, input + wcslen(input), back_inserter(utf8line));
    utf8line.push_back(0);
    
    return &utf8line[0];
}

wchar_t* char_to_wide(const char* input)
{
    static std::vector<wchar_t> utf16line;
    utf16line.clear();
    
    utf8::utf8to16(input, input + strlen(input), back_inserter(utf16line));
    utf16line.push_back(0);
    
    return &utf16line[0];
}


std::wstring narrow_to_wide(const std::string& mbs)
{
    return char_to_wide(mbs.c_str());
}

std::string wide_to_narrow(const std::wstring& wcs)
{
    return std::string(wide_to_char(wcs.c_str()));
}

wchar_t* wgettext(const char *str)
{
    return char_to_wide(str);
}
std::wstring wstrgettext(std::string text)
{
    return narrow_to_wide(text);
}
// Get an sha-1 hash of the player's name combined with
// the password entered. That's what the server uses as
// their password. (Exception : if the password field is
// blank, we send a blank password - this is for backwards
// compatibility with password-less players).
std::string translatePassword(std::string playername, std::wstring password)
{
	if(password.length() == 0)
		return "";

	std::string slt = playername + wide_to_narrow(password);
	SHA1 sha1;
	sha1.addBytes(slt.c_str(), slt.length());
	unsigned char *digest = sha1.getDigest();
	std::string pwd = base64_encode(digest, 20);
	free(digest);
	return pwd;
}

size_t curl_write_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
    std::ostringstream *stream = (std::ostringstream*)userdata;
    size_t count = size * nmemb;
    stream->write(ptr, count);
    return count;
}

u32 readFlagString(std::string str, FlagDesc *flagdesc) {
	u32 result = 0;
	char *s = &str[0];
	char *flagstr, *strpos = NULL;
	
	while ((flagstr = strtok_r(s, ",", &strpos))) {
		s = NULL;
		
		while (*flagstr == ' ' || *flagstr == '\t')
			flagstr++;
		
		for (int i = 0; flagdesc[i].name; i++) {
			if (!strcasecmp(flagstr, flagdesc[i].name)) {
				result |= flagdesc[i].flag;
				break;
			}
		}
	}
	
	return result;
}

std::string writeFlagString(u32 flags, FlagDesc *flagdesc) {
	std::string result;
	
	for (int i = 0; flagdesc[i].name; i++) {
		if (flags & flagdesc[i].flag) {
			result += flagdesc[i].name;
			result += ", ";
		}
	}
	
	size_t len = result.length();
	if (len >= 2)
		result.erase(len - 2, 2);
	
	return result;
}

char *mystrtok_r(char *s, const char *sep, char **lasts) {
	char *t;

	if (!s)
		s = *lasts;

	while (*s && strchr(sep, *s))
		s++;

	if (!*s)
		return NULL;

	t = s;
	while (*t) {
		if (strchr(sep, *t)) {
			*t++ = '\0';
			break;
		}
		t++;
	}
	
	*lasts = t;
	return s;
}

u64 read_seed(const char *str) {
	char *endptr;
	u64 num;
	
	if (str[0] == '0' && str[1] == 'x')
		num = strtoull(str, &endptr, 16);
	else
		num = strtoull(str, &endptr, 10);
		
	if (*endptr)
		num = murmur_hash_64_ua(str, (int)strlen(str), 0x1337);
		
	return num;
}
