/*
 *      Author: venture
 *		用于缓存池, 固定缓存池大小
 *		只能顺序移除，不能随机移除
 */
#pragma once
#include <stdint.h>
#include "stl/set_list.h"

template <typename F, typename T, size_t NUM_M = 1>
class set_fix : public set_list<F, T>
{
public:
	set_fix() { }
	~set_fix() { }

	bool isfull()
	{
		return set_list<F, T>::size() >= _maxsize;
	}

	void setmax(size_t num)
	{
		_maxsize = num;
	}

private:
	size_t			_maxsize = NUM_M;
};

