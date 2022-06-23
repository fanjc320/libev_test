/*
 *
 *      Author: venture
 */
#pragma once
#include "net/NetBase.h"

struct evconnlistener;
typedef void (*NET_LISTENER_CB)(struct evconnlistener*, SOCKET_T, struct sockaddr*, int socklen, void*);

class NetEventBase;

class NetListener
{ 
public:
    enum EListenFlag
    {
        ELF_NONE = 0,
        ELF_REUSEABLE = 1,
        ELF_CLOSE_ON_FREE = 2,
    };

    void SetListener(evconnlistener* p);
    int Disable();
    void Free();
    SOCKET_T GetFd();
private:
    evconnlistener* _pListener = nullptr;
};

extern NetListener* net_listener_new_bind(NetEventBase* base, NET_LISTENER_CB cb, void* ptr, int backlog, const struct sockaddr* sa, int socklen, bool bReuseable, bool bCloseOnFree);