/*
 *
 *      Author: venture
 */
#pragma once

#include <string>

namespace char_set
{
	extern std::string	GBK_UTF8(const std::string& src);
	extern std::string	UTF8_GBK(const std::string& src);
	extern std::wstring UTF8_UCS2(const std::string& src);
	extern std::string	UCS2_UTF8(const std::wstring& src);

	extern int			GetUtf8Length(const char* utf8str);
	extern void			ToUnicodeBytes(const std::wstring& src, std::string& strDest);
	extern std::wstring BytesToUnicode(const std::string& src);
};
