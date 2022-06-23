/*
 *      Author: venture
 */

#pragma once

#include <stdint.h>
#include <vector>
#include "dtype.h"

using namespace std;
template <typename F, typename T, size_t NUM_M, size_t MOD_NUM = 1>
class map_mod
{
public:
	map_mod() {}
	~map_mod() {}

	size_t getindex(F nIndex)
	{
		size_t i = (nIndex / MOD_NUM) % NUM_M;
		return i;
	}

	void insert(F nIndex, const T& value)
	{
        T* p = find(nIndex);
        if (p != nullptr)
        {
            if (*p == value) return true;
            LOG_ERROR("exsit!!");
            return true;
        }

		size_t i = getindex(nIndex);
		(_hashmap[i])[nIndex] = value;
	}

	bool exsit(F nIndex)
	{
		size_t i = getindex(nIndex);
		return _hashmap[i].find(nIndex) != _hashmap[i].end();
	}

	void erase(F nIndex)
	{
		size_t i = getindex(nIndex);
		_hashmap[i].erase(nIndex);
	}

	T* find(F nIndex)
	{
		size_t i = getindex(nIndex);
		auto itr = _hashmap[i].find(nIndex);
		if (itr != _hashmap[i].end())
			return &(itr->second);

		return nullptr;
	}

	void clear()
	{
		for (size_t i = 0; i < NUM_M; ++i)
		{
			_hashmap[i].clear();
		}
	}

	umap<F, T>& getmap(size_t nIndex)
	{
		return _hashmap[nIndex];
	}

	umap<F, T>& tickmap()
	{
		size_t index = _pos % NUM_M;
		_pos = index + 1;
		return _hashmap[index];
	}

	void getindex(std::vector<F>& vecIndex)
	{
		for (size_t i = 0; i < NUM_M; ++i)
		{
			for (auto itr = _hashmap[i].begin(); itr != _hashmap[i].end(); ++itr)
				vecIndex.push_back(itr->first);
		}
	}

	size_t size()
	{
		size_t nSize = 0;
		for (size_t i = 0; i < NUM_M; ++i)
		{
			nSize += _hashmap[i].size();
		}
		return nSize;
	}

	size_t indexnum()
	{
		return NUM_M;
	}

private:
	umap<F, T>  _hashmap[NUM_M];
	size_t		_pos = 0;
};