/*
 *
 *      Author: venture
 */

#include "pch.h"
#include "TimeCount.h"
#include "time_func_x.h"
#include "math/math_func.h"

GenCount::GenCount()
{
    _value = 0;
    _interval = 0;
}

GenCount::~GenCount(void)
{
    _value = 0;
    _interval = 0;
}

int64_t GenCount::InterVal()
{
    return _interval;
}

void GenCount::Init(int64_t interval)
{
    _interval = interval;
    _value = 0;
}

void GenCount::Reset()
{
    _value = 0;
}

int64_t GenCount::Now()
{
    return _value;
}

bool GenCount::On(int64_t add)
{
    _value += add;
    if (_value >= _interval)
    {
        _value = 0;
        return true;
    }
    return false;
}

TimeTickCount::TimeTickCount()
{
    _tm_begin = get_curr_time();
    _tm_end = _tm_begin;
}

int64_t TimeTickCount::Begin()
{
    return _tm_begin;
}

void TimeTickCount::Reset()
{
    _tm_begin = get_curr_time();
    _tm_end = _tm_begin;
}

int64_t TimeTickCount::Ms()
{
    _tm_end = get_curr_time();
    return _tm_end - _tm_begin;
}

RdtscCount::RdtscCount()
{
    _ulbegin = RDTSC();
    _ulend = _ulbegin;
}

int64_t RdtscCount::Begin()
{
    return _ulbegin;
}

void RdtscCount::Reset()
{
    _ulbegin = RDTSC();
    _ulend = _ulbegin;
}

int64_t RdtscCount::Count()
{
    _ulend = RDTSC();
    return _ulend - _ulbegin;
}