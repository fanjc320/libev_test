/*
 *
 *      Author: venture
 */
#pragma once
#include <mutex>

#include "net/base/CirQueue.h"

/************************************************************************/
//多进多出
class CirQueueGen : public CirQueue
{
public:
    CirQueueGen() { }
    ~CirQueueGen() { Release(); }

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
};
