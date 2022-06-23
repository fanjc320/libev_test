/*
 *
 *      Author: venture
 */
#pragma once

#include <string>
using namespace std;
#include "dtype.h"

class WordTree;
class WordTreeNode
{
public:
	friend class WordTree;
	typedef umap<wchar_t, WordTreeNode*> _TreeMap;
	typedef umap<wchar_t, WordTreeNode*>::iterator _TreeMapIterator;

	WordTreeNode(const wchar_t& character, const WordTreeNode* parent = nullptr);
	WordTreeNode();
	~WordTreeNode();

	wchar_t GetText() const;
	WordTreeNode* FindChild(const wchar_t& nextCharacter);
	WordTreeNode* InsertChild(const wchar_t& nextCharacter);

	bool IsNode() const { return _bnode; }
	void SetNode(bool val) { _bnode = val; }
private:
	wchar_t 		        _text = 0;
	const WordTreeNode*		_parent = nullptr;
	_TreeMap 		        _map;
	bool                    _bnode = false;
};