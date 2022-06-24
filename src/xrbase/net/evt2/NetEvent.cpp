/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "NetEvent.h"
#include "event2/event_struct.h"
#include "event2/event.h"

#include "NetEventBase.h"
#include "log/LogMgr.h"

NetEvent::NetEvent()
{
    if (_pEvent == nullptr)
        _pEvent = new event();
}

NetEvent::~NetEvent()
{
    if (_pEvent != nullptr)
    {
        delete _pEvent;
    }
}

int NetEvent::Assign(NetEventBase* pEventBase, SOCKET_T fd, short events, NET_EVENT_CALLBACK_FN callback, void* callback_arg)
{
    short evts = 0;
    if (events & EET_PERSIST)
    {
        evts |= EV_PERSIST;
    }
    if (events & EET_READ)
    {
        evts |= EV_READ;
    }
    if (events & EET_WRITE)
    {
        evts |= EV_WRITE;
    }
    _pEventBase = pEventBase;
    LOG_MINE("mine", "fd:%d", fd);
    return event_assign(_pEvent, _pEventBase->EventBase(), fd, evts, callback, callback_arg);
}

int NetEvent::Add(const struct timeval* timeout)
{
    return event_add(_pEvent, timeout);
}

int NetEvent::Del()
{
    return event_del(_pEvent);
}
