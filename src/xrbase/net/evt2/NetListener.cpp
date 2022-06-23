/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "NetListener.h"
#include "event2/listener.h"
#include "net/evt2/NetEventBase.h"
#include "log/LogMgr.h"

void NetListener::SetListener(evconnlistener* p)
{
    _pListener = p;
}

SOCKET_T NetListener::GetFd()
{
    return  evconnlistener_get_fd (_pListener);
}

int NetListener::Disable()
{
    return evconnlistener_disable(_pListener);
}

void NetListener::Free()
{
    evconnlistener_free(_pListener);
}

NetListener* net_listener_new_bind(NetEventBase* base, NET_LISTENER_CB cb, void* ptr, int backlog, const struct sockaddr* sa, int socklen, bool bReuseable, bool bCloseOnFree)
{
    unsigned fs = 0;
    if (bReuseable)
    {
        fs |= LEV_OPT_REUSEABLE;
    }
    if (bCloseOnFree)
    {
        fs |= LEV_OPT_CLOSE_ON_FREE;
    }

    evconnlistener* p = evconnlistener_new_bind(base->EventBase(), cb, ptr, fs, backlog, sa, socklen);
    if (p == nullptr)
    {
        LOG_ERROR("error!!");
        return nullptr;
    }
    NetListener* pListener = new NetListener();
    pListener->SetListener(p);
    return pListener;
}
