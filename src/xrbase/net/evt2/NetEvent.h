/*
 *
 *      Author: venture
 */
#pragma once
#include "net/NetBase.h"

struct event;
typedef void (*NET_EVENT_CALLBACK_FN)(SOCKET_T, short, void*);

class NetEventBase;
class NetEvent
{ 
public:
    NetEvent();
    ~NetEvent();
    enum EEvtType
    {
        EET_NONE = 0,
        EET_PERSIST = 1,
        EET_WRITE = 2,
        EET_READ = 4,
    };
   int Assign(NetEventBase* pEventBase, SOCKET_T fd, short events, NET_EVENT_CALLBACK_FN callback, void* callback_arg);
   int Add(const struct timeval* timeout);
   int Del();

private:
    event* _pEvent = nullptr;
    NetEventBase* _pEventBase = nullptr;
};

