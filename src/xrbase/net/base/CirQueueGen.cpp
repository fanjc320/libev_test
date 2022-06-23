/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <mutex>
#include "CirQueueGen.h"
#include "log/LogMgr.h"

/************************************************************************/
bool CirQueueGen::Init(uint32_t max_count)
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

    _head = 0;
    _tail = 0;
    _cur_count = 0;
    _max_count = max_count;

    return true;
}

/************************************************************************/
bool CirQueueGen::Release()
{
    std::unique_lock<std::mutex> lck(_mutex);

    if (_elems != nullptr)
        delete[] _elems;

    _head = 0;
    _tail = 0;
    _cur_count = 0;
    _max_count = 0;
    _elems = nullptr;
    return true;
}

/************************************************************************/
int CirQueueGen::Push(const ElemP elem)
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

void CirQueueGen::SerPop()
{

}
/************************************************************************/
CirQueueGen::ElemP CirQueueGen::Pop()
{
    _mutex.lock();
    if (_cur_count <= 0)
    {
        _mutex.unlock();
        return nullptr;
    }
    ElemP elem = *(_elems + _head);
    _elems[_head] = nullptr;
    _head = _head + 1;
    if (_head >= _max_count)
    {
        _head = 0;
    }
    _cur_count = _cur_count - 1;
    _mutex.unlock();
    return elem;
}

/************************************************************************/
int32_t CirQueueGen::Size()
{
    return _cur_count;
}

/************************************************************************/
bool CirQueueGen::Empty()
{
    return _cur_count == 0;
}

/************************************************************************/
bool CirQueueGen::Full()
{
    return _cur_count == _max_count;
}

//如果满了，将cq清空
bool CirQueueGen::Clear()
{
    _head = 0;
    _tail = 0;
    _cur_count = 0;

    return true;
}