/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "NetEventBase.h"
#include "event2/event.h"
#include "log/LogMgr.h"

int NetEventBase::event_loopbreak()
{
    if (_pEvBase != nullptr)
    {
        return event_base_loopbreak(_pEvBase);
    }
    return -1;
}

int NetEventBase::event_dispatch()
{
    if (_pEvBase != nullptr)
    {
        LOG_MINE("mine", "", "");

        return event_base_dispatch(_pEvBase);
    }
    return -1;
}

NetEventBase* net_event_new()
{
    event_base* pEv = event_base_new();
    if (pEv == nullptr)
    {
        LOG_ERROR("error");
        return nullptr;
    }
    NetEventBase* pNetBase = new NetEventBase();
    if (pNetBase == nullptr)
    {
        LOG_ERROR("error");
        event_base_free(pEv);
        return nullptr;
    }
    pNetBase->SetEventBase(pEv);
    return pNetBase;
}

void net_event_free(NetEventBase* pEvBase)
{
    if (pEvBase == nullptr) return;
    event_base_free(pEvBase->EventBase());
    delete pEvBase;
}
