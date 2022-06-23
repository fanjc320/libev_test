/*
 *
 *      Author: venture
 */
#pragma once

#include <functional>
#include "net/NetBase.h"
#include "net/evt2/NetEvent.h"

struct event;
typedef std::function<void(uint64_t)> FUNC_EVENT_TIMEOUT;

class NetEventBase;
class EventTimer
{
public:
    EventTimer();
    EventTimer(NetEventBase* pEvBase, bool bPersist, uint64_t iMilliSecs, FUNC_EVENT_TIMEOUT func);
    virtual ~EventTimer();
    virtual void OnTimer();

    bool Init(NetEventBase* pEvBase, bool bPersist, uint64_t iMilliSecs, FUNC_EVENT_TIMEOUT func);

    bool Init();
    void Stop();
    uint64_t Ms();

private:
    bool _CreateTimer();

    bool            _persist = true;  
    bool            _bRun = false;
    uint64_t        _millisecs = 0;
    FUNC_EVENT_TIMEOUT  _func = nullptr;
    NetEventBase*   _pEvbase = nullptr;
    NetEvent*       _pEvent = nullptr;
    struct timeval  _tval;
};