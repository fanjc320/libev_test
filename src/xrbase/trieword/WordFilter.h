/*
 *
 *      Author: venture
 */
#pragma once

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class WordTree;
class WordFilter
{
public:
	WordFilter();
	virtual ~WordFilter();

private:
	WordTree* _pTree = nullptr;

public:
	bool Init();
	bool Init(const char* fileName);
	bool Clear();

	//必须为utf8字节
	bool AddWordUft8(const string& wutf8);
	//检查敏感词并且替换为**
	std::string Censor(const string& source);
	//检查是否通过，有敏感词返回false
	bool IsCensor(const string& source);

private:
	bool _initialized = false;

	std::wstring _Utf8ToUnicode(const std::string& strSrc);
	string _StrToLower(const string& str);
};
