/*
 *
 *      Author: venture
 */
#pragma once

#include <vector>
#include <thread>

#include "net/NetSetting.h"
#include "net/base/EventThread.h"
#include "net/base/MsgQueue.h"
#include "net/base/NetConnPool.h"

class	IoThreadControl;
class	IoThreadControl;
struct	AcceptConnMsg;
struct	ConnMsg;
class   IoThread;

class IoThread
{
public:
    IoThread();
    ~IoThread();

    int Init(thread_oid_t iOid, const NetSetting& conf,
        IoThreadControl* pThreadCtl, EventThread* pLogicThread);
    void Fini();
    void Stop();

    EventThread* EvThread();

    MsgQueue& InPut();
    MsgQueue& OutPut();
    MsgQueue& AcceptPut();

    NetConnPool* ConnPool();
    IoThreadControl* GetParent();

    void AddPut() { ++_nPutOutCount; }
    void AddGet() { ++_nGetOutCount; }
    std::string name = "";
private:
    EventThread* _pEvThread = nullptr;

    uint32_t _nPutOutCount = 0;
    uint32_t _nGetOutCount = 0;

    NetConnPool        _connPool;
    IoThreadControl*  _pParent = nullptr;
    bool               _bStop = false;

    MsgQueue _InputQueue;
    MsgQueue _OutputQueue;
    MsgQueue _AcceptConnQueue;
    NetSetting _setting;

    void _OnMsgAcceptFunc(const event_msg_t msg);
    void _OnMsgInputFunc(const event_msg_t msg);
    void _OnMsgOutputFunc(const event_msg_t msg);
    void _OnIoHandleConnMsg(ConnMsg* pConnMsg);
};