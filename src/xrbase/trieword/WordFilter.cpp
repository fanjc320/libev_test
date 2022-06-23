/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "WordFilter.h"
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include "log/LogMgr.h"
#include "text/char_set.h"
#include "WordTree.h"

 //屏蔽替换的字符

#define SHIELD_WCHAR L"*"
#define SHIELD_CHAR  "*"

WordFilter::WordFilter()
{
	_pTree = new WordTree;
}

WordFilter::~WordFilter()
{
	if (_pTree)
	{
		delete _pTree;
		_pTree = new WordTree;
	}
}


std::wstring WordFilter::_Utf8ToUnicode(const std::string& strSrc)
{
	return char_set::UTF8_UCS2(strSrc);
}

string WordFilter::_StrToLower(const string& str)
{
	std::string strTmp;
	strTmp.resize(str.size());
	std::transform(str.begin(), str.end(), strTmp.begin(), ::tolower);
	return strTmp;
}

bool WordFilter::Init()
{
	_initialized = false;

	if (_pTree == nullptr)
		_pTree = new WordTree;
	if (_pTree == nullptr)
		return false;

	_initialized = true;
	return true;
}

bool WordFilter::Init(const char* filePath)
{
	_initialized = false;

	if (!setlocale(LC_ALL, "zh_CN.UTF-8"))
	{
		printf("locale failed!\n");
		return false;
	}

	if (_pTree == nullptr)
		_pTree = new WordTree;
	if (_pTree == nullptr)
		return false;

	ifstream keywordsFile(filePath);
	if (keywordsFile.is_open())
	{
		cout << "load trieword:" << filePath << std::endl;

		const int LINE_LENGTH = 512;
		char str[LINE_LENGTH];
		size_t nLen = 0;
		string strText;
		wstring strDest;
		int nCount = 0;
		while (keywordsFile.getline(str, LINE_LENGTH))
		{
			nLen = strlen(str);
			if (nLen > 0)
			{
				if (str[nLen - 1] == '\r')
				{
					str[nLen - 1] = '\0';
					--nLen;
				}

				strText.assign(str, nLen);
				strText = _StrToLower(strText);
				strDest = _Utf8ToUnicode(strText);
				_pTree->Insert(strDest);
				strText.clear();
				strDest.clear();
			}
			++nCount;
		}
		cout << "load trieword end ! count = " << nCount << std::endl;
	}
	else
	{
		cout << "load trieword error :" << filePath << std::endl;
		return false;
	}

	keywordsFile.close();
	_initialized = true;
	return true;
}

bool WordFilter::Clear()
{
	_initialized = false;
	if (_pTree != nullptr)
	{
		delete _pTree;
		_pTree = nullptr;
	}
	_pTree = new WordTree;
	if (_pTree == nullptr)
		return false;

	_initialized = true;
	return true;
}

bool WordFilter::AddWordUft8(const string& wutf8)
{
	if (!_initialized)
	{
		LOG_WARNING("not Init!");
		return true;
	}

	std::string strText = _StrToLower(wutf8);
	std::wstring strDest = _Utf8ToUnicode(strText);
	if (strDest.empty()) return false;
	_pTree->Insert(strDest);
	return true;
}

std::string WordFilter::Censor(const string& source)
{
	if (!_initialized)
	{
		LOG_WARNING("not Init!");
		return source;
	}
	else
	{
		wstring strDest = _Utf8ToUnicode(_StrToLower(source));
		wstring strRet = _Utf8ToUnicode(source);

		if (strDest.size() != strRet.size()) return SHIELD_CHAR;
		//不能正确转化出unicode的输出错误*
		if (strDest.empty()) return SHIELD_CHAR;

		size_t length = strDest.size();
		for (size_t i = 0; i < length; ++i)
		{
			wstring substring = strDest.substr(i, length - i);
			if (_pTree->Find(substring) != nullptr)    //发现敏感词
			{
				//替换为*--002A
				strDest.replace(i, _pTree->GetCount(), SHIELD_WCHAR);
				strRet.replace(i, _pTree->GetCount(), SHIELD_WCHAR);
				if (strRet.size() != strDest.size()) return SHIELD_CHAR;
				length = strDest.size();
			}
		}
		if (strRet.empty()) return SHIELD_CHAR;
		return char_set::UCS2_UTF8(strRet);
	}
}

bool WordFilter::IsCensor(const string& source)
{
	if (!_initialized)
	{
		LOG_WARNING("not Init!");
		return true;
	}
	else
	{
		wstring strDest = _Utf8ToUnicode(_StrToLower(source));
		//不能正确转化出unicode的输出错误
		if (strDest.empty()) return false;

		size_t length = strDest.size();
		for (size_t i = 0; i < length; ++i)
		{
			wstring substring = strDest.substr(i, length - i);
			if (_pTree->Find(substring) != nullptr)    //发现敏感词
			{
				return false;
			}
		}
	}
	return true;
}
