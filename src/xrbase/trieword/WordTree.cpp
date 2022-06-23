/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "WordTree.h"

WordTree::WordTree()
{
	_pEmptyRoot = new WordTreeNode();
}

WordTree::~WordTree()
{
	if (_pEmptyRoot != nullptr)
		delete _pEmptyRoot;

	_pEmptyRoot = nullptr;
}

WordTreeNode* WordTree::Insert(wstring& keyword)
{
	if (keyword.empty()) return nullptr;

	return Insert(_pEmptyRoot, keyword);
}

WordTreeNode* WordTree::Insert(const wchar_t* keyword)
{
	if (keyword == nullptr) return nullptr;
	wstring wordstr(keyword);
	if (wordstr.empty()) return nullptr;

	return Insert(wordstr);
}

WordTreeNode* WordTree::Insert(WordTreeNode* parent, wstring& keyword)
{
	if (parent == nullptr) return nullptr;

	if (keyword.size() == 0)
	{
		parent->SetNode(true);
		return nullptr;
	}
	wchar_t firstChar = *keyword.begin();
	WordTreeNode* firstNode = parent->FindChild(firstChar);

	if (firstNode == nullptr)
		return InsertBranch(parent, keyword);

	wstring restChar = keyword.substr(1, keyword.size());
	return Insert(firstNode, restChar);
}

WordTreeNode* WordTree::InsertBranch(WordTreeNode* parent, wstring& keyword)
{
	if (keyword.size() == 0 || parent == nullptr)
	{
		return nullptr;
	}

	wchar_t firstChar = *keyword.begin();
	WordTreeNode* firstNode = parent->InsertChild(firstChar);
	if (firstNode != nullptr)
	{
		wstring restChar = keyword.substr(1, keyword.size());
		if (!restChar.empty())
			return InsertBranch(firstNode, restChar);
		else
		{
			firstNode->SetNode(true);
			return firstNode;
		}
	}
	return nullptr;
}

WordTreeNode* WordTree::Find(wstring& keyword)
{
	_count = 0;
	return Find(_pEmptyRoot, keyword);
}

WordTreeNode* WordTree::Find(WordTreeNode* parent, wstring& keyword)
{
	if (keyword.size() == 0 || parent == nullptr)
	{
		_count = 0;
		return nullptr;
	}

	wchar_t firstChar = *keyword.begin();
	WordTreeNode* firstNode = parent->FindChild(firstChar);
	if (firstNode == nullptr)            //未找到
	{
		_count = 0;
		return nullptr;
	}

	++_count;
	//对应词组结束，则判断该词为敏感词
	if (firstNode->_map.empty() || firstNode->IsNode())
	{
		return firstNode;
	}

	wstring restChar = keyword.substr(1, keyword.size());

	if (restChar.empty() || keyword.size() == 1)//最后一个字
	{
		if (firstNode->IsNode())
			return firstNode;
		else
			return nullptr;
	}

	return Find(firstNode, restChar);
}
