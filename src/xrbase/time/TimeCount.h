/*
 *
 *      Author: venture
 */
#pragma once

#include <time.h>
#include <stdint.h>
#include "dtype.h"

//普通计数
class GenCount
{
public:
    GenCount();
    virtual ~GenCount(void);

    int64_t InterVal();
    void Init(int64_t interval);
    void Reset();
    int64_t Now();
    bool On(int64_t add = 1);

protected:
	int64_t _value = 0;
	int64_t _interval = 0;
};

//数量计数
template <int64_t TICKS>
class NumCount : public GenCount
{
public:
	NumCount()
	{
		_value = 0;
		_interval = TICKS;
	}
	virtual ~NumCount(void)
	{
		_value = 0;
		_interval = TICKS;
	}
};

//秒计数
template <time_t TICKSEC>
class TimeCount
{
public:
	TimeCount()
	{
		_tmBegin = 0;
        _interval = TICKSEC;
	}
	TimeCount(time_t tmBegin)
	{
		_tmBegin = tmBegin;
        _interval = TICKSEC;
	}
	virtual ~TimeCount(void)
	{
		_tmBegin = 0;
	}

    void Init(time_t tmBegin, time_t interval)
    {
        _tmBegin = tmBegin;
        _interval = interval;
    }

    void ResetBegin(time_t tmNow)
    {
        _tmBegin = tmNow;
    }

	bool On(time_t tmNow)
	{
        if (_tmBegin == 0)
            _tmBegin = tmNow;

		if (tmNow >= (time_t)(_tmBegin + _interval))
		{
			_tmBegin = tmNow;
			return true;
		}
		return false;
	}
protected:
	time_t _tmBegin = 0;
    time_t _interval = 0;
};

//获得毫秒
class TimeTickCount
{
public:
    TimeTickCount();
	int64_t Begin();
    void Reset();
    //获得毫秒
	int64_t Ms();

private:
	int64_t _tm_begin;
	int64_t _tm_end;
};

//获得CPU的周期
class RdtscCount
{
public:
    RdtscCount();

	int64_t Begin();
    void Reset();
    //获得CPU的周期
	int64_t Count();

public:
	int64_t _ulbegin;
	int64_t _ulend;
};
