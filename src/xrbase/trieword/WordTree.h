/*
 * Tree.h
 *
 *      Author: venture
 */
#pragma once
#include<iostream>
#include <string>

#include "trieword/WordTreeNode.h"

class WordTreeNode;
using namespace std;

class WordTree
{
public:
	WordTree();
	~WordTree();

	WordTreeNode* Insert(wstring& keyword);
	WordTreeNode* Insert(const wchar_t* keyword);
	WordTreeNode* Find(wstring& keyword);

	int GetCount() const
	{
		return _count;
	}

private:
	int _count = 0;    //当前查找的一个敏感词的字数
	WordTreeNode* _pEmptyRoot = nullptr;

	WordTreeNode* Insert(WordTreeNode* parent, wstring& keyword);
	WordTreeNode* InsertBranch(WordTreeNode* parent, wstring& keyword);
	WordTreeNode* Find(WordTreeNode* parent, wstring& keyword);
};
