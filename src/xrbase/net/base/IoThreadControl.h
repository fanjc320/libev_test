/*
 *
 *      Author: venture
 */
#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "net/NetSetting.h"
#include "net/NetBase.h"

struct NetMsg;
struct AcceptConnMsg;
class IoThreadControl;
class IoThread;
class EventThread;
class LogicHandle;

typedef thread_oid_t(*FUN_DISPATCH_THREAD)(IoThreadControl* pDispatcher);

class IoThreadControl
{
public:
    IoThreadControl();
    ~IoThreadControl();

    int Init(EventThread* pLogicThread, const NetSetting& threadconf);
    int Start();
    int Fini();
    int Stop();

    int DispatchAccpet(SOCKET_T fd, session_oid_t soid, sockaddr addr
        , uint32_t maxBuffer, EConnType accepted = EConnType::ACCEPTED);
    int DispatchConnect(SOCKET_T fd, session_oid_t soid, sockaddr addr
        , uint32_t maxBuffer);

    thread_oid_t GetThreads();
    conn_oid_t GetThreadMaxConn();

    void RegLogicHandle(LogicHandle* pLogicHandle);
    LogicHandle* GetLogicHandle();

    IoThread* GetIoThread(thread_oid_t toid);
    bool SendMsgHton(void* pNetMsg);

    bool IsValidIoThread(thread_oid_t iThreadOid);
    void InstallDispatchFun(FUN_DISPATCH_THREAD fn);

    void  AddInitCount();
    LogicHandle* GetInterFace() { return _pLogicHandle; }
    std::string name = "";
private:
    static thread_oid_t _DispatchDefault(IoThreadControl* pThreadCtl);
    static thread_oid_t _DispatchMin(IoThreadControl* pThreadCtl);
    std::mutex       _lockInit;
    std::condition_variable _condInit;

    IoThread*    _pIoThreads = nullptr;
    thread_oid_t _iThreadNum = 0;
    thread_oid_t _iThreadPos = 0;

    FUN_DISPATCH_THREAD _fnDispatch = nullptr;
    conn_oid_t   _nMaxConnOfThread = 0;
    LogicHandle* _pLogicHandle = nullptr;

    thread_oid_t _iCurInitCount = 0;
};