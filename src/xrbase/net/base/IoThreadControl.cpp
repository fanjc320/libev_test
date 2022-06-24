/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "IoThreadControl.h"

#include <vector>
#include <thread>
#include <mutex> // std::mutex

#include "time/time_func_x.h"
#include "MsgQueue.h"
#include "NetConn.h"
#include "MsgConn.h" 
#include "net/LogicHandle.h"
#include "net/EventTimer.h"
#include "log/LogMgr.h"
#include "EventThread.h"
#include "IoThread.h"
#include "container/fix_id_alloc.h"

unsigned SP_THREAD_CALL io_thread_fn(void* args)
{
    IoThread* pThread = (IoThread*) args;

    IoThreadControl* pThreadCtl = pThread->GetParent();
    pThreadCtl->AddInitCount();
    LOG_MINE("mine", "", "");

    pThread->EvThread()->MainLoop();
    return 0;
}

IoThreadControl::IoThreadControl()
{

}

IoThreadControl::~IoThreadControl()
{
    Fini();
}

int IoThreadControl::Init(EventThread* pLogicThread, const NetSetting& conf)
{
    if (!conf.IsValid())
    {
        return -1;
    }

    _pIoThreads = new IoThread[conf.ioNum];
    if (_pIoThreads == nullptr)
    {
        return -1;
    }
    for (thread_oid_t i = 0; i < conf.ioNum; ++i)
    {
        _pIoThreads[i].name = this->name + "_io" + std::to_string(i);
        if (_pIoThreads[i].Init(i, conf, this, pLogicThread) != 0)
            return -1;
    }
    _iThreadNum = conf.ioNum;
    _iThreadPos = 0;
    _fnDispatch = &IoThreadControl::_DispatchDefault;

    _nMaxConnOfThread = conf.connsOfIo;
    _pLogicHandle = nullptr;

    _iCurInitCount = 0;
    return 0;
}

int IoThreadControl::Stop()
{
    if (_pIoThreads != nullptr)
    {
        for (thread_oid_t i = 0; i < _iThreadNum; ++i)
        {
            _pIoThreads[i].Stop();
        }
    }
    if (_pLogicHandle != nullptr)
        _pLogicHandle->Stop();
    return 0;
}

int IoThreadControl::Fini()
{
    if (_pIoThreads != nullptr)
    {
        for (thread_oid_t i = 0; i < _iThreadNum; ++i)
        {
            _pIoThreads[i].Fini();
        }
        delete[] _pIoThreads;
        _pIoThreads = nullptr;
    }
    _iThreadNum = 0;
    _iThreadPos = 0;
    _pLogicHandle = nullptr;

    _iCurInitCount = 0;

    return 0;
}

thread_oid_t IoThreadControl::_DispatchDefault(IoThreadControl* pThreadCtl)
{
    if (pThreadCtl->_iThreadPos >= pThreadCtl->_iThreadNum)
        pThreadCtl->_iThreadPos = 0;
    return pThreadCtl->_iThreadPos++;
}

thread_oid_t IoThreadControl::_DispatchMin(IoThreadControl* pThreadCtl)
{
    thread_oid_t pos = 0;
    size_t connsize = 0;
    size_t tempsize = 0;
    for (thread_oid_t i = 0; i < pThreadCtl->_iThreadNum; ++i)
    {
        tempsize = pThreadCtl->GetIoThread(i)->ConnPool()->ConnSize();
        if (tempsize == 0)
        {
            return i;
        }

        if (connsize == 0 || tempsize < connsize)
        {
            pos = i;
            connsize = tempsize;
        }
    }
    return pos;
}

void IoThreadControl::AddInitCount()
{
    _lockInit.lock();
    ++_iCurInitCount;
    _condInit.notify_all();
    _lockInit.unlock();
}

void IoThreadControl::InstallDispatchFun(FUN_DISPATCH_THREAD fn)
{
    _fnDispatch = fn;
}

bool IoThreadControl::IsValidIoThread(thread_oid_t iThreadOid)
{
    return (iThreadOid >= 0 && iThreadOid < _iThreadNum);
}

int IoThreadControl::Start()
{
    for (thread_oid_t i = 0; i < _iThreadNum; ++i)
    {
        IoThread* pThread = &_pIoThreads[i];
        if (pThread->EvThread()->Linked())
            pThread->GetParent()->AddInitCount();
        else
        {
            LOG_MINE("mine", "", "");
            pThread->EvThread()->Start(&io_thread_fn, (void*)pThread);
        }
    }

    std::unique_lock<std::mutex> lck(_lockInit);
    while (_iCurInitCount < _iThreadNum)
    {
        _condInit.wait(lck);
    }

    return 0;
}

int IoThreadControl::DispatchAccpet(SOCKET_T fd, session_oid_t soid
    , sockaddr addr, uint32_t maxBuffer, EConnType accepted)
{
    thread_oid_t iThreadOid = (*_fnDispatch)(this);
    if (!IsValidIoThread(iThreadOid))
    {
        return -1;
    }
    IoThread& ioThread = _pIoThreads[iThreadOid];
    MsgQueue& MsgQueue = ioThread.AcceptPut();//唯一调用AcceptPut的地方

    AcceptConnMsg* pConn = acc_conn_alloc();
    if (pConn == nullptr) return -1;
    pConn->fd = fd;
    pConn->connType = accepted;
    pConn->soid = soid;
    pConn->maxBuffer = maxBuffer;
    pConn->addr = addr;
    LOG_MINE("mine", "fd:%d", fd);
    if (!MsgQueue.Push((void*) pConn))
    {
        LOG_ERROR("accept!");
        MSG_FREE(pConn);
        return -1;
    }
    return 0;
}

int IoThreadControl::DispatchConnect(SOCKET_T fd, session_oid_t soid, sockaddr addr
    , uint32_t maxBuffer)
{
    return DispatchAccpet(fd, soid, addr, maxBuffer, EConnType::RECONN);
}

void IoThreadControl::RegLogicHandle(LogicHandle* pLogicHandle)
{
    if (pLogicHandle)
    {
        pLogicHandle->Initialize(this, _iThreadNum, _nMaxConnOfThread);
        _pLogicHandle = pLogicHandle;
    }
}

LogicHandle* IoThreadControl::GetLogicHandle()
{
    return _pLogicHandle;
}

bool IoThreadControl::SendMsgHton(void* pNetMsg)
{
    if (pNetMsg == nullptr) return false;
    HostHd* pHost = (HostHd*) pNetMsg;
    thread_oid_t toid = pHost->threadOid;

    if (!IsValidIoThread(toid))
    {
        LOG_ERROR("send_msg_hton(is_valid_io_thread=%u)", (uint32_t) toid);
        return false;
    }
    IoThread& ioThread = _pIoThreads[toid];
    ioThread.AddPut();
    MsgQueue& OutputQueue = ioThread.OutPut();
    if (!(OutputQueue.Push(pNetMsg)))
    {
        LOG_ERROR("error!");
        return false;
    }
    return true;
}

thread_oid_t IoThreadControl::GetThreads()
{
    return _iThreadNum;
}

IoThread* IoThreadControl::GetIoThread(thread_oid_t toid)
{
    if (IsValidIoThread(toid))
    {
        return &_pIoThreads[toid];
    }
    return nullptr;
}

conn_oid_t IoThreadControl::GetThreadMaxConn()
{
    return _nMaxConnOfThread;
}
