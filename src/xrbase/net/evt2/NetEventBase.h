/*
 *
 *      Author: venture
 */
#pragma once

struct event_base;
class NetEventBase
{
public:
    event_base* EventBase() { return _pEvBase; }
    void SetEventBase(event_base* pEv) { _pEvBase = pEv; }

    int event_loopbreak();
    int event_dispatch();
private:
    event_base* _pEvBase = nullptr;
};

extern NetEventBase* net_event_new();
extern void net_event_free(NetEventBase* pEvBase);