/*
 *
 *      Author: venture
 */
#pragma once
#include <mutex>

#include "net/base/CirQueue.h"
/************************************************************************/
//多进一出

class CirQueueMul : public CirQueue
{
public:
    CirQueueMul() { }
    ~CirQueueMul() { Release(); }

    bool        Init(uint32_t max_count);
    bool        Release();
    int         Push(const ElemP elem);
    void        SerPop();
    ElemP		Pop();
    int32_t     Size();
    bool        Empty();
    bool        Full();
    bool        Clear();

private:
    std::mutex  _mutex;
    int32_t     _pop = 0;
    int32_t     _pop_count = 0;
    ElemP*      _elemspop = nullptr;
};
