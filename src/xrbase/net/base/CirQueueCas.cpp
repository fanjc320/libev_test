/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "log/LogMgr.h"
#include "CirQueueCas.h"
#include <cstring>
 /************************************************************************/
bool CirQueueCas::Init(uint32_t max_count)
{
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
bool CirQueueCas::Release()
{
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
int CirQueueCas::Push(const ElemP elem)
{
    if (elem == nullptr)
        return -1;

    if (_cur_count >= _max_count)
    {
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
        return 1;

    return 2;
}

void CirQueueCas::SerPop()
{

}
/************************************************************************/
CirQueueCas::ElemP CirQueueCas::Pop()
{
    if (_cur_count <= 0)
    {
        return nullptr;
    }
    ElemP elem = *(_elems + _head);
    if (elem == nullptr)
    {
        LOG_ERROR("error!!");
        return nullptr;
    }
    _elems[_head] = nullptr;
    _head = _head + 1;
    if (_head >= _max_count)
    {
        _head = 0;
    }
    if (_cur_count > 0)
        _cur_count = _cur_count - 1;
    else
    {
        _cur_count = 0;
        LOG_ERROR("error!!");
    }

    return elem;
}

/************************************************************************/
int32_t CirQueueCas::Size()
{
    return _cur_count;
}

/************************************************************************/
bool CirQueueCas::Empty()
{
    return _cur_count == 0;
}

/************************************************************************/
bool CirQueueCas::Full()
{
    return _cur_count == _max_count;
}

//如果满了，将cq清空
bool CirQueueCas::Clear()
{
    _head = 0;
    _tail = 0;
    _cur_count = 0;

    return true;
}