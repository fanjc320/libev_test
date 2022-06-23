/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "EventTimer.h"
#include "evt2/NetEventBase.h"
#include "evt2/NetEvent.h"
#include "dtype.h"
#include <cstring>

static void timeout_cb(SOCKET_T fd, short event, void* arg)
{
    EventTimer* pTimeEvent = (EventTimer*) arg;
    pTimeEvent->OnTimer();
}

EventTimer::EventTimer(NetEventBase* pEvBase, bool bPersist, uint64_t iMilliSecs, FUNC_EVENT_TIMEOUT func)
    : _persist(bPersist)
    , _millisecs(iMilliSecs)
    , _func(func)
{
    _pEvbase = pEvBase;
    _CreateTimer();
}

EventTimer::EventTimer()
{
    memset(&_tval, 0, sizeof(_tval));
}

EventTimer::~EventTimer()
{
    Stop();
}

void EventTimer::OnTimer()
{
    if (_func != nullptr)
        _func(_millisecs);
}

bool EventTimer::Init(NetEventBase* pEvBase, bool bPersist, uint64_t iMilliSecs, FUNC_EVENT_TIMEOUT func)
{
    Stop();

    _pEvbase = pEvBase;
    _persist = bPersist;
    _millisecs = iMilliSecs;
    _func = func;

    if (!_CreateTimer()) return false;

    if (_pEvent->Add(&_tval) == 0)
    {
        _bRun = true;
        return true;
    }
    return false;
}


bool EventTimer::Init()
{
    if (_pEvent->Add(&_tval) == 0)
    {
        _bRun = true;
        return true;
    }
    return false;
}

void EventTimer::Stop()
{
    if (_bRun)
    {
        _pEvent->Del();
        _bRun = false;
        SAFE_DELETE(_pEvent);
    }
}

uint64_t EventTimer::Ms()
{
    return _millisecs;
}

bool EventTimer::_CreateTimer()
{
    short nFlags = 0;
    if (_persist)
    {
        nFlags |= NetEvent::EET_PERSIST;
    }

    _tval.tv_sec = (long)(_millisecs / 1000);
    _tval.tv_usec = (long)(_millisecs % 1000) * 1000;

    if (_pEvent == nullptr)
        _pEvent = new NetEvent();

    int r = _pEvent->Assign(_pEvbase, -1, nFlags, timeout_cb, (void*) this);
    return (r == 0);
}
