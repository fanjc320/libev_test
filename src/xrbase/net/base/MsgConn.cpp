/*
 *
 *      Author: venture
 */
#include "pch.h"

#include "MsgConn.h"
#include "ListenThread.h"
#include "NetConn.h"
#include "MsgQueue.h"
#include "IoThread.h"
#include "IoThreadControl.h"
#include "log/LogMgr.h"

int io_notify_logic_conn_msg(NetConn* pConn, EConnState state, session_oid_t soid)
{
    IoThread* pThread = pConn->GetThread();
    if (pThread)
    {
        LOG_MINE("mine", "", " conn_thid:%d connid:%d", pConn->GetThreadId(), pConn->GetConnId());
        MsgQueue& InputQueue = pThread->InPut();

        ConnMsg* pConnMsg = conn_msg_alloc();
        if (pConnMsg == nullptr) return -1;
        pConnMsg->hd.type = EHostMtType::CONN;
        pConnMsg->hd.threadOid = pConn->GetThreadId();
        pConnMsg->hd.connOid = pConn->GetConnId();
        pConnMsg->body.state = state;
        pConnMsg->body.soid = soid;
        pConnMsg->body.maxBuffer = 0;
        if (InputQueue.Push(pConnMsg))
        {
            return 0;
        }
        else
        {
            LOG_ERROR("error!");
            MSG_FREE(pConnMsg);
        }
    }
    return -1;
}

int logic_notify_io_conn_msg(IoThreadControl* pThreadCtl, const HostHd& hd,
    EConnState state, session_oid_t soid, uint32_t maxBuffer)
{
    IoThread* pThread = pThreadCtl->GetIoThread(hd.threadOid);
    if (pThread == nullptr)
    {
        return -1;
    }
    LOG_MINE("mine", "", " hd_thid:%d hd_connid:%d", hd.threadOid, hd.connOid);
    MsgQueue& OutputQueue = pThread->OutPut();

    ConnMsg* pConnMsg = conn_msg_alloc();
    if (pConnMsg == nullptr) return -1;

    pConnMsg->hd.type = EHostMtType::CONN;
    pConnMsg->hd.threadOid = hd.threadOid;
    pConnMsg->hd.connOid = hd.connOid;
    pConnMsg->body.state = state;
    pConnMsg->body.soid = soid;
    pConnMsg->body.maxBuffer = maxBuffer;
    if (OutputQueue.Push(pConnMsg))
    {
        return 0;
    }
    else
    {
        LOG_ERROR("error!");
        MSG_FREE(pConnMsg);
    }

    return -1;
}

int logic_notify_io_conn_msg(IoThreadControl* pThreadCtl, EConnState state)
{
    thread_oid_t num = pThreadCtl->GetThreads();
    HostHd hd;
    for (thread_oid_t i = 0; i < num; ++i)
    {
        hd.type = EHostMtType::CONN;
        hd.threadOid = i;
        logic_notify_io_conn_msg(pThreadCtl, hd, state, 0, 0);
    }
    return 0;
}

int logic_notify_listen_conn_msg(ListenThread* pListen, EConnState state)
{
    MsgQueue& OutputQueue = pListen->OutPut();

    ConnMsg* pConnMsg = conn_msg_alloc();
    if (pConnMsg == nullptr) return -1;

    pConnMsg->hd.type = EHostMtType::CONN;
    pConnMsg->body.state = state;
    LOG_MINE("mine", "", " msg_hd_thid:%d msg_hd_connid:%d", pConnMsg->hd.threadOid, pConnMsg->hd.connOid);
    if (OutputQueue.Push(pConnMsg))
    {
        return 0;
    }
    else
    {
        LOG_ERROR("error!");
        MSG_FREE(pConnMsg);
    }

    return -1;
}

ConnMsg* conn_msg_alloc()
{
    uint8_t* p = (uint8_t*) msg_alloc(NET_CONN_SIZE);
    if (p == nullptr) return nullptr;

    ConnMsg* pMsg = new (p)ConnMsg();
    return pMsg;
}

AcceptConnMsg* acc_conn_alloc()
{
    uint8_t* p = (uint8_t*) msg_alloc(NET_ACCEPT_SIZE);
    if (p == nullptr) return nullptr;

    AcceptConnMsg* pMsg = new (p)AcceptConnMsg();
    return pMsg;
}
