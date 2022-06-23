/*
 *
 *      Author: venture
 */
#pragma once

#include "net/base/CirQueue.h"

/************************************************************************/
//一进一出，无锁
class CirQueueCas : public CirQueue
{
public:
    CirQueueCas() { }
    ~CirQueueCas() { Release(); }

    bool        Init(uint32_t max_count);
    bool        Release();
    int         Push(const ElemP elem);
    void        SerPop();
    ElemP		Pop();
    int32_t     Size();
    bool        Empty();
    bool        Full();
    bool        Clear();
};