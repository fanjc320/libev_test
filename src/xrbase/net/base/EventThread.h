/*
 *
 *      Author: venture
 */
#pragma once

#include <vector>
#include <thread>
#include <functional>

#include "net/NetBase.h"

typedef std::function<unsigned(void*)> FUN_EVENT_THREAD;

class NetEventBase;
class EventThread
{
public:
    EventThread();
    ~EventThread();

    int Init(thread_oid_t iOid);
    int Init(thread_oid_t iOid, NetEventBase* pEvBase);
    int Fini();

    bool Linked() const { return _bLink; }

    thread_oid_t Id();
    NetEventBase* Base();

    int Start(FUN_EVENT_THREAD fnThread, void* args);
    int Stop();
    int MainLoop();
    std::string name = "";
private:
    thread_oid_t    _id = INVALID_THREAD_OID;
    NetEventBase*   _pEvBase = nullptr;
    std::thread     _thread;
    bool            _bLink = false;
    bool            _bStop = false;
};
