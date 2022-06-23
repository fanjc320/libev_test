/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "IoThread.h"
#include "NetConn.h"

#include "EventThread.h"
#include "NetConn.h"
#include "MsgConn.h"
#include "MsgQueue.h"
#include "net/LogicHandle.h"
#include "IoThreadControl.h"
#include "log/LogMgr.h"
#include "net/NetBase.h"
#include "net/base/sock_opt.h"
#include "net/MsgParser.h"
#include "net/MsgHead.h"

IoThread::IoThread()
{ }

IoThread::~IoThread()
{
    Fini();
}

static FUN_IO_SEND_MSG g_io_send_msg_fn = io_send_msg_x;
//////////////////////////////////////////////////////////////////////////
void regfn_io_send_msg(FUN_IO_SEND_MSG fn)
{
    if (fn) g_io_send_msg_fn = fn;
}

int IoThread::Init(thread_oid_t iOid, const NetSetting& conf,
    IoThreadControl* pThreadCtl, EventThread* pLogicThread)
{
    if (iOid == INVALID_THREAD_OID || conf.connsOfIo <= 0)
    {
        return -1;
    }

    _nPutOutCount = 0;
    _nGetOutCount = 0;
    _pEvThread = new EventThread;
    if (_pEvThread == nullptr)
        return -1;

    if (conf.CirType() == ECirType::ONE)
    {
        if (_pEvThread->Init(iOid, pLogicThread->Base()) != 0)
            return -1;
    }
    else
    {
        if (_pEvThread->Init(iOid) != 0)
            return -1;
    }

    if (!_connPool.Init(this, conf.connsOfIo))
        return -1;
    _AcceptConnQueue.name = this->name+"_accept";
    // listen_thread to io_thread
    if (!(_AcceptConnQueue.Init(conf.acceptMaxSize, _pEvThread->Base(),
        std::bind(&IoThread::_OnMsgAcceptFunc, this, std::placeholders::_1)
        , conf.CirType())))
    {
        return -1;
    }
    _InputQueue.name = this->name + "_input";
    // io_thread to LogicThread
    if (!(_InputQueue.Init(conf.inMaxSize, pLogicThread->Base(),
        std::bind(&IoThread::_OnMsgInputFunc, this, std::placeholders::_1)
        , conf.CirType())))
    {
        return -1;
    }
    _OutputQueue.name = this->name + "_output";
    // CLogicThread to io_thread
    if (!(_OutputQueue.Init(conf.outMaxSize, _pEvThread->Base(),
        std::bind(&IoThread::_OnMsgOutputFunc, this, std::placeholders::_1)
        , conf.CirType())))
    {
        return -1;
    }

    if (!_connPool.StartTimer(conf.detectSec))
        return -1;

    _pParent = pThreadCtl;
    _setting = conf;
    _bStop = false;
    return 0;
}

void IoThread::Fini()
{
    _bStop = true;
    _pParent = nullptr;
    _connPool.Release();

    _AcceptConnQueue.Release();
    _InputQueue.Release();
    _OutputQueue.Release();
    if (_pEvThread != nullptr)
        delete _pEvThread;
    _pEvThread = nullptr;
}

void IoThread::Stop()
{
    if (_bStop) return;
    _bStop = true;

    _AcceptConnQueue.Stop();
    _InputQueue.Stop();
    _OutputQueue.Stop();

    if (_pEvThread != nullptr)
        _pEvThread->Stop();
}

EventThread* IoThread::EvThread()
{
    return _pEvThread;
}

void IoThread::_OnMsgAcceptFunc(const event_msg_t msg)
{
    AcceptConnMsg* pAccept = (AcceptConnMsg*) msg;
    if (pAccept == nullptr)
    {
        return;
    }

    NetConn* pNewConn = ConnPool()->NewConn(pAccept->maxBuffer);
    if (pNewConn == nullptr)
    {
        sock_opt::CloseSocket(pAccept->fd);
        LOG_ERROR("the conn count is over limit ip = %s", sock_opt::Inet_Ip(pAccept->addr).c_str());
        return;
    }

    if (pNewConn->Init(pAccept->fd, EvThread()->Base()
        , pAccept->addr, pAccept->connType, _setting.secRecvLimit) != 0)
    {
        LOG_ERROR("net_conn_init failed ip = %s", sock_opt::Inet_Ip(pAccept->addr).c_str());
        sock_opt::CloseSocket(pAccept->fd);
        ConnPool()->DelConn(pNewConn->GetConnId());
        return;
    }

    EConnState state = EConnState::NET_ACCEPTED;
    if (pAccept->connType == EConnType::RECONN)
        state = EConnState::NET_CONNECTED;

    pNewConn->AddRecvSec();
    LOG_MINE("mine", "", "");
    if (io_notify_logic_conn_msg(pNewConn, state, pAccept->soid) == -1)
    {
        LOG_ERROR("io_notify failed ip = %s", sock_opt::Inet_Ip(pAccept->addr).c_str());
        ConnPool()->DelConn(pNewConn->GetConnId());
    }

    LOG_CUSTOM("accept", "net_conn_init connect ip = %s", sock_opt::Inet_Ip(pAccept->addr).c_str())
}

void IoThread::_OnIoHandleConnMsg(ConnMsg* pConnMsg)
{
    LOG_MINE("mine", "", "");
    if (pConnMsg->body.state == EConnState::LAYER_DISCONN)
    {
        ConnPool()->DelConn(pConnMsg->hd.connOid);
    }
    else if (pConnMsg->body.state == EConnState::LAYER_KILL)
    {
        ConnPool()->KillConn(pConnMsg->hd.connOid);
    }
    else if (pConnMsg->body.state == EConnState::LAYER_CONNECTED)
    {
        NetConn* pConn = ConnPool()->FindConn(pConnMsg->hd.connOid);
        if (pConn)
        {
            pConn->SetMaxBuffer(pConnMsg->body.maxBuffer);
        }
    }
    else if (pConnMsg->body.state == EConnState::NET_EXIT)
    {
        Stop();
    }
}

void IoThread::_OnMsgInputFunc(const event_msg_t msg)
{
    LogicHandle* pHandle = GetParent()->GetLogicHandle();

    HostHd* pHostMsgHd = (HostHd*) msg;
    if (pHostMsgHd->type == EHostMtType::CONN)
    {
        LOG_MINE("mine", "", "");
        pHandle->OnHandleConnMsg(msg);
    }
    else
    {
        LOG_MINE("mine", "", "");
        pHandle->OnHandleLogicMsg((NetMsg*)msg);
    }
}

void IoThread::_OnMsgOutputFunc(const event_msg_t msg)
{
    LOG_MINE("mine", "", "");
    HostHd* pHostMsgHd = (HostHd*) msg;
    if (pHostMsgHd->type == EHostMtType::CONN)
    {
        ConnMsg* pConnMsg = (ConnMsg*) msg;
        _OnIoHandleConnMsg(pConnMsg);
    }
    else
    {
        AddGet();

        if (g_io_send_msg_fn != nullptr)
            (*g_io_send_msg_fn)(msg, this);
    }
}
MsgQueue& IoThread::InPut()
{
    LOG_MINE("mine", "", "");
    //LOG_MY("test", "", "");
    return _InputQueue;
}

MsgQueue& IoThread::OutPut()
{
    LOG_MINE("mine", "", "");
    return _OutputQueue;
}

MsgQueue& IoThread::AcceptPut()
{
    LOG_MINE("mine", "", "");
    return _AcceptConnQueue;
}

NetConnPool* IoThread::ConnPool()
{
    return &_connPool;
}
IoThreadControl* IoThread::GetParent()
{
    return _pParent;
}
