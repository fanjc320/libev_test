/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <mutex>
#include "CirQueueMul.h"
#include "log/LogMgr.h"

/************************************************************************/
bool CirQueueMul::Init(uint32_t max_count)
{
    std::unique_lock<std::mutex> lck(_mutex);

    if (max_count == 0)
    {
        return false;
    }

    _elems = new ElemP[max_count];
    memset(_elems, 0, sizeof(ElemP) * max_count);
    if (_elems == nullptr)
    {
        return false;
    }
    _elemspop = new ElemP[max_count];
    memset(_elemspop, 0, sizeof(ElemP) * max_count);
    if (_elemspop == nullptr)
    {
        return false;
    }

    _tail = 0;
    _cur_count = 0;
    _max_count = max_count;

    return true;
}

/************************************************************************/
bool CirQueueMul::Release()
{
    std::unique_lock<std::mutex> lck(_mutex);

    if (_elems != nullptr)
        delete[] _elems;
    if (_elemspop != nullptr)
        delete[] _elemspop;

    _tail = 0;
    _cur_count = 0;
    _max_count = 0;
    _elems = nullptr;
    _elemspop = nullptr;
    return true;
}

/************************************************************************/
int CirQueueMul::Push(const ElemP elem)
{
    _mutex.lock();
    if (_cur_count >= _max_count)
    {
        _mutex.unlock();
        return -1;
    }

    _elems[_tail] = elem;
    _tail = _tail + 1;
    if (_tail >= _max_count)
    {
        _tail = 0;
    }
    _cur_count = _cur_count + 1;
    if (_cur_count == 1)
    {
        _mutex.unlock();
        return 1;
    }
    _mutex.unlock();
    return 2;
}

void  CirQueueMul::SerPop()
{
    _mutex.lock();
    if (_cur_count <= 0)
    {
        _mutex.unlock();
        return;
    }
    std::swap(_elems, _elemspop);
    //    memcpy(_elemspop, _elems, sizeof(cq_elem_t)*_tail);
    _pop_count = _cur_count;
    _pop = 0;

    _tail = 0;
    _cur_count = 0;
    _mutex.unlock();
}

/************************************************************************/
CirQueueMul::ElemP CirQueueMul::Pop()
{
    if (_pop_count <= 0)
        return nullptr;

    ElemP elem = *(_elemspop + _pop);
    _elemspop[_pop] = nullptr;
    _pop = _pop + 1;

    if (_pop >= _pop_count)
    {
        _pop_count = 0;
        _pop = 0;
    }

    return elem;
}

/************************************************************************/
int32_t CirQueueMul::Size()
{
    return _cur_count;
}

/************************************************************************/
bool CirQueueMul::Empty()
{
    return _cur_count == 0;
}

/************************************************************************/
bool CirQueueMul::Full()
{
    return _cur_count == _max_count;
}

//如果满了，将cq清空
bool CirQueueMul::Clear()
{
    _tail = 0;
    _cur_count = 0;

    return true;
}