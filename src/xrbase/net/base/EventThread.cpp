/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "EventThread.h"
#include "net/NetBase.h"
#include "log/LogMgr.h"
#include "net/evt2/NetEventBase.h"

 //////////////////////////////////////////////////////////////////////////

EventThread::EventThread()
{ }

EventThread::~EventThread()
{
    Fini();
}

int EventThread::Init(thread_oid_t iOid)
{
    _pEvBase = net_event_new();//唯一调用net_event_new的地方
    if (_pEvBase == nullptr)
    {
        return -1;
    }
    _id = iOid;
    _bStop = false;
    return 0;
}

int EventThread::Init(thread_oid_t iOid, NetEventBase* pEvBase)
{
    _pEvBase = pEvBase;
    _id = iOid;
    _bLink = true;
    _bStop = false;
    return 0;
}

int EventThread::Fini()
{
    _bStop = true;
    if (_pEvBase == nullptr)
        return 0;

    if (_pEvBase != nullptr)
    {
        if (!_bLink) net_event_free(_pEvBase);
        _pEvBase = nullptr;
    }

    _id = INVALID_THREAD_OID;
    if (_thread.joinable())
        _thread.join();

    return 0;
}

thread_oid_t EventThread::Id()
{
    return _id;
}

NetEventBase* EventThread::Base()
{
    return _pEvBase;
}

int EventThread::Start(FUN_EVENT_THREAD fnThread, void* args)
{
    if (!_bLink)
    {
        LOG_MINE("mine", "name:%s", name.c_str());
        _thread = std::thread(fnThread, args);//唯一调用std::thread的地方
        LOG_MINE("mine", "name:%s _thread:%d", name.c_str(), _thread.get_id());
    }
    return 0;
}

int EventThread::Stop()
{
    if (_bStop) return 0;
    _bStop = true;

    if (_bLink) return 0;
    if (_pEvBase != nullptr)
    {
        int ret = _pEvBase->event_loopbreak();
        LOG_INFO("ret:%d", ret);
        return ret;
    }
    return -1;
}

int EventThread::MainLoop()
{
    if (_bLink) return 0;

    if (_pEvBase != nullptr)
    {
        LOG_MINE("mine", "", "");

        return _pEvBase->event_dispatch();
    }
    return -1;
}