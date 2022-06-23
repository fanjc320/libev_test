/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "dtype.h"
#include <string.h>
#include "char_set.h"
#include <stdio.h>
#include <iostream>
#include <string.h>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <iconv.h>

#endif // _MSC_VER

namespace char_set
{
	const unsigned char UTF8BYTE[] = { 0x00,0x00,0x06,0x0E,0x1E,0x3E,0x7E };
#define UTF8CHECK(c, i, v) (((c>>i)&v)==v)
#define UTF8CHECKEX(c, i)  UTF8CHECK(c, (7-i), UTF8BYTE[i])

	std::string GBK_UTF8(const std::string& src)
    {		
		std::string strRet;
        if(src.empty()) return strRet;
#if (defined _MSC_VER)
		int len = 0;
		char* dst = nullptr;
		LPWSTR utf8 = nullptr;

		len = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, nullptr, 0);
		utf8 = (LPWSTR) malloc(sizeof(wchar_t) * len + 1);
		if (!utf8) return strRet;

		MultiByteToWideChar(CP_ACP, 0, (LPCTSTR) src.c_str(), -1, utf8, len);
		len = WideCharToMultiByte(CP_UTF8, 0, utf8, -1, nullptr, 0, nullptr, nullptr);
		dst = (char*) malloc((size_t) len + 1);
		if (!dst)
		{
			free(utf8);
			return strRet;
		}
		WideCharToMultiByte(CP_UTF8, 0, utf8, -1, dst, len, nullptr, nullptr);
		dst[len] = '\0';
        strRet.append(dst, len);		
		free(utf8);
		free(dst);
		return strRet;
#else
		iconv_t cd;
		char* utf8_data = nullptr;
		size_t content_len = 0, utf8_data_len = 0;

		content_len = src.length();
		if (content_len == 0)
			return strRet;

		cd = iconv_open("utf8", "gb2312");
		if (cd == 0)
			return strRet;

		utf8_data_len = content_len * 4 + 1;
		utf8_data = (char*) malloc(utf8_data_len);
		if (!utf8_data)
			return strRet;

		memset(utf8_data, 0, utf8_data_len);

		char* in = (char*) src.c_str();
		char* out = utf8_data;

		if (iconv(cd, &in, (size_t*) &content_len, &out, (size_t*) &utf8_data_len) == -1)
		{
			iconv_close(cd);
			free(utf8_data);
			return strRet;
		}

		iconv_close(cd);
		strRet.append(utf8_data, utf8_data_len);
		free(utf8_data);
		return strRet;
#endif
	}

	std::string UTF8_GBK(const std::string& src)
	{
        std::string strRet;
        if (src.empty()) return strRet;
#if (defined _MSC_VER)
		int len = 0;
		char* dst = nullptr;
		LPWSTR ansi = nullptr;

		len = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, nullptr, 0);
		ansi = (LPWSTR) malloc(sizeof(wchar_t) * len + 1);
		if (!ansi) return strRet;

		MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR) src.c_str(), -1, ansi, len);
		len = WideCharToMultiByte(CP_ACP, 0, ansi, -1, nullptr, 0, nullptr, nullptr);

		dst = (char*) malloc((size_t) len + 1);
		if (!dst)
		{
			free(ansi);
			return strRet;
		}

		WideCharToMultiByte(CP_ACP, 0, ansi, -1, dst, len, nullptr, nullptr);
		dst[len] = '\0';
		strRet.append(dst, len);
		free(ansi);
		free(dst);
		return strRet;
#else
		iconv_t cd;
		char* ansi_data = nullptr;
		size_t content_len = 0, ansi_data_len = 0;

		content_len = src.length();
		if (content_len == 0)
			return strRet;

		cd = iconv_open("gb2312", "utf8");
		if (cd == 0)
			return strRet;

		ansi_data_len = content_len * 4 + 1;
		ansi_data = (char*) malloc(ansi_data_len);
		if (!ansi_data)
			return strRet;

		memset(ansi_data, 0, ansi_data_len);

		char* in = (char*) src.c_str();
		char* out = ansi_data;

		if (iconv(cd, &in, (size_t*) &content_len, &out, (size_t*) &ansi_data_len) == -1)
		{
			iconv_close(cd);
			free(ansi_data);
			return strRet;
		}

		iconv_close(cd);
		strRet.append(ansi_data, ansi_data_len);
		free(ansi_data);
		return strRet;
#endif
	}

	std::wstring UTF8_UCS2(const std::string& src)
	{
		std::wstring strRet;
		if (src.empty()) return strRet;
#if (defined _MSC_VER)
		int len = 0;
		LPWSTR utf8 = nullptr;

		len = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, nullptr, 0);
		utf8 = (LPWSTR) malloc(sizeof(wchar_t) * len + 1);
		if (!utf8) return strRet;

		MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR) src.c_str(), (int) src.length(), utf8, len);
		strRet.append(utf8, len);
		free(utf8);
		return strRet;
#else
		iconv_t cd;
		char* utf8_data = nullptr;
		size_t content_len = 0, utf8_data_len = 0;

		content_len = src.length();
		if (content_len == 0) return strRet;

		cd = iconv_open("utf8", "UNICODE//IGNORE");
		if (cd == 0) return strRet;

		utf8_data_len = content_len * sizeof(wchar_t) + 1;
		utf8_data = (char*) malloc(utf8_data_len);
		if (!utf8_data) return strRet;

		memset(utf8_data, 0, utf8_data_len);

		char* in = (char*) src.c_str();
		char* out = utf8_data;

		if (iconv(cd, &in, (size_t*) &content_len, &out, (size_t*) &utf8_data_len) == -1)
		{
			iconv_close(cd);
			free(utf8_data);
			return strRet;
		}

		iconv_close(cd);
		strRet.append((wchar_t*) utf8_data, utf8_data_len / 4);
		free(utf8_data);
		return strRet;
#endif
	}

	std::string UCS2_UTF8(const std::wstring& src)
	{
		std::string strRet;
		if (src.empty()) return strRet;
#if (defined _MSC_VER)
		int len = 0;
		char* dst = nullptr;

		len = WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, nullptr, 0, nullptr, nullptr);
		dst = (LPSTR) malloc(sizeof(char) * len);
		if (!dst) return strRet;

		WideCharToMultiByte(CP_UTF8, 0, src.c_str(), (int) src.length(), dst, len, nullptr, nullptr);
		dst[len] = '\0';
		strRet.append(dst, len);
		free(dst);
		return strRet;
#else
		iconv_t cd;
		char* ansi_data = nullptr;
		size_t content_len = 0, ansi_data_len = 0;

		content_len = src.length();
		if (content_len == 0) return strRet;

		cd = iconv_open("UNICODE//IGNORE", "utf8");
		if (cd == 0) return strRet;

		ansi_data_len = content_len * 4 + 1;
		ansi_data = (char*) malloc(ansi_data_len);
		if (!ansi_data) return strRet;

		memset(ansi_data, 0, ansi_data_len);

		char* in = (char*) src.c_str();
		char* out = ansi_data;

		if (iconv(cd, &in, (size_t*) &content_len, &out, (size_t*) &ansi_data_len) == -1)
		{
			iconv_close(cd);
			free(ansi_data);
			return strRet;
		}

		iconv_close(cd);
		strRet.append(ansi_data, ansi_data_len);
		free(ansi_data);
		return strRet;
#endif
	}

	/* 获取UTF8字符串的长度,对于多字节字符也按1个字符计算,最大只处理64KB字符串
	** 必须确保是UTF8字符串，否则会导致出错
	*/
	int GetUtf8Length(const char* utf8str)
	{
		int length = 0;
		char* pc = (char*) utf8str;
		char c = *pc;
		while ((c != 0) && (length < 65535))
		{
			if (UTF8CHECKEX(c, 6))
			{
				length++;
				pc += 6;
			}
			else if (UTF8CHECKEX(c, 5))
			{
				length++;
				pc += 5;
			}
			else if (UTF8CHECKEX(c, 4))
			{
				length++;
				pc += 4;
			}
			else if (UTF8CHECKEX(c, 3))
			{
				//三字节
				length++;
				pc += 3;
			}
			else if (UTF8CHECKEX(c, 2))
			{
				//双字节
				length++;
				pc += 2;
			}
			else
			{
				//单个字符
				length++;
				pc++;
			}
			c = *pc;
		}
		return length;
	};

	void ToUnicodeBytes(const std::wstring& src, std::string& strDest)
 	{
		size_t wLen = src.length();
		const wchar_t* pSrc = src.c_str();

		char szResult[12];
		for (unsigned int i = 0; i < wLen; ++i)
		{
#ifdef _MSC_VER
			sprintf(szResult, "%04X", pSrc[i]);
			strDest.append(szResult, 4);
#else
			sprintf(szResult, "%08X", pSrc[i]);
			strDest.append(szResult, 8);
#endif // _MSC_VER
		}
	}

	std::wstring BytesToUnicode(const std::string& src)
	{
        unsigned int wWord = 4;		
		std::wstring strDest;

#ifdef _MSC_VER
		wWord = 4;
#else
		wWord = 8;
#endif // _MSC_VER
		if (src.size() % wWord != 0)
			return strDest;

		size_t Len = src.length();

		for (unsigned int i = 0; i < Len; i = i + wWord)
		{
			std::string strSub = src.substr(i, wWord);
			wchar_t wChar = (wchar_t) std::stoul(strSub.c_str(), 0, 16);
			strDest.append(1, wChar);
		}

		return strDest;
	}

}