/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <iostream>
#include "WordTreeNode.h"
using namespace std;

WordTreeNode::WordTreeNode()
{
}

WordTreeNode::~WordTreeNode()
{
	for (_TreeMapIterator itr = _map.begin(); itr != _map.end(); ++itr)
	{
		delete itr->second;
	}
	_map.clear();
}

wchar_t WordTreeNode::GetText() const
{
	return _text;
}

WordTreeNode::WordTreeNode(const wchar_t& character, const WordTreeNode* parent)
{
	_text = character;
	_parent = parent;
}


WordTreeNode* WordTreeNode::FindChild(const wchar_t& nextCharacter)
{
	_TreeMapIterator itr = _map.find(nextCharacter);
	if (itr != _map.end())
		return itr->second;

	return nullptr;
}

WordTreeNode* WordTreeNode::InsertChild(const wchar_t& nextCharacter)
{
	if (!FindChild(nextCharacter))    //添加节点，并返回节点位置
	{
		WordTreeNode* pNode = new WordTreeNode(nextCharacter, this);
		_map.insert(_TreeMap::value_type(nextCharacter, pNode));

		return pNode;
	}
	return nullptr;
}
