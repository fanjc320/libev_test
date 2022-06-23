/*
 *
 *      Author: venture
 */
#pragma once
#include <mutex>

/************************************************************************/
//多进多出
class CirQueue
{
public:
    typedef void* ElemP;

    CirQueue() { }
    virtual ~CirQueue() { }

    virtual bool    Init(uint32_t max_count) = 0;
    virtual bool    Release() = 0;
    virtual int     Push(const ElemP elem) = 0;
    virtual void    SerPop() = 0;
    virtual ElemP	Pop() = 0;
    virtual int32_t Size() = 0;
    virtual bool    Empty() = 0;
    virtual bool    Full() = 0;
    virtual bool    Clear() = 0;

protected:
    volatile int32_t        _head = 0;
    volatile int32_t        _tail = 0;
    volatile int32_t        _cur_count = 0;
    int32_t                 _max_count = 0;
    ElemP* _elems = nullptr;
};