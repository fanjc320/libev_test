/*
 *      Author: venture
 *		用于缓存池, 固定缓存池大小
 *		只能顺序移除，不能随机移除
 */

#pragma once
#include <stdint.h>
#include "stl/map_list.h"

template <typename F, typename T, size_t NUM_M = 1>
class map_fix : public map_list<F, T>
{
public:
	map_fix() {}
	~map_fix() {}

	bool isfull()
	{
		return map_list<F, T>::size() >= _maxsize;
	}

	void setmax(size_t num)
	{
		_maxsize = num;
	}
private:
	size_t			_maxsize = NUM_M;
};

