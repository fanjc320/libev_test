/*
 *
 *      Author: venture
 */
#pragma once

class LuaItem
{
public:
	LuaItem()
	{
		_nKey = -1;
	}
	virtual ~LuaItem()
	{
	}

	void SetKey(int nLuaKey) { _nKey = nLuaKey; }
	int  GetKey() const { return _nKey; }

private:
	int _nKey = 0;
};