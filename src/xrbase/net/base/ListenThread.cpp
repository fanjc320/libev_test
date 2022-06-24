/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <mutex> // std::mutex
#ifndef _MSC_VER
#include <linux/tcp.h>
#endif

#include "ListenThread.h"
#include "MsgQueue.h"
#include "NetConn.h"
#include "MsgConn.h" 
#include "net/LogicHandle.h"
#include "net/EventTimer.h"
#include "log/LogMgr.h"
#include "EventThread.h"
#include "IoThread.h"
#include "IoThreadControl.h"
#include "net/NetBase.h"
#include "net/base/sock_opt.h"
#include "net/evt2/NetEventBase.h"
#include "net/evt2/NetListener.h"

ListenThread::ListenThread()
{ }

ListenThread::~ListenThread()
{
    Fini();
}

//////////////////////////////////////////////////////////////////////////
void _ListenCallBack(struct evconnlistener* pListener, SOCKET_T fd,
    struct sockaddr* pSockAddr, int socklen, void* user_data)
{
    ListenThread* pListenThread = (ListenThread*) user_data;
    IoThreadControl* pThreadCtl = pListenThread->Dispatcher();
    if (pThreadCtl == nullptr)
    {
        sock_opt::CloseSocket(fd);
        return;
    }
    LOG_MINE("mine", "fd:%d", fd);
    if (pThreadCtl->DispatchAccpet(fd, INVALID_SESSION_OID
        , *pSockAddr, 0) == -1)
    {
        sock_opt::CloseSocket(fd);
    }
}

unsigned SP_THREAD_CALL ListenThread::_ListenThreadFunc(void* args)
{
    ListenThread* pThread = (ListenThread*) args;
    if (pThread->EvThread() != nullptr)
        pThread->EvThread()->MainLoop();

    return 0;
}

int ListenThread::_AddListen(const char* szIp, uint16_t port)
{
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    if (szIp == nullptr || strlen(szIp) == 0)
        sin.sin_addr.s_addr = inet_addr("0.0.0.0");
    else
        sin.sin_addr.s_addr = inet_addr(szIp);
    sin.sin_port = htons(port);

    NetListener* pListener = net_listener_new_bind(_pEvThread->Base(),
        _ListenCallBack, this, -1,
        (struct sockaddr*) &sin,
        sizeof(sin), true, true);

    if (!pListener)
    {
        return -1;
    }

    SOCKET_T fd = pListener->GetFd();
    sock_opt::SetCloseExec(fd);
    LOG_MINE("mine", "fd:%d", fd);
    if (!sock_opt::SetNoBlock(fd))
    {
        LOG_ERROR(" nonblocking  client: %d error:%d, %s", fd, errno
            , strerror(errno));
    }

    _pListener = pListener;
    _port = port;

    return 0;
}

int ListenThread::Init(EventThread* pLogicThread, const NetSetting& conf, IoThreadControl* pThreadCtl)
{
    _pEvThread = new EventThread;
    _pEvThread->name = this->name + "_event";
    if (conf.CirType() == ECirType::ONE)
    {
        if (_pEvThread->Init(INVALID_THREAD_OID, pLogicThread->Base()) != 0)
        {
            LOG_ERROR("EventThread Init failed (port:%u)", conf.port);
            return -1;
        }
    }
    else
    {
        if (_pEvThread->Init(INVALID_THREAD_OID) != 0)
        {
            LOG_ERROR("EventThread Init failed (port:%u)", conf.port);
            return -1;
        }
    }

    // logic to listen
    if (!(_OutputQueue.Init(64, _pEvThread->Base(),
        std::bind(&ListenThread::_OnMsgOutFun, this, std::placeholders::_1)
        , conf.CirType())))
    {
        return -1;
    }

    if (_AddListen(conf.ip.c_str(), conf.port) != 0)
    {
        LOG_ERROR("AddListen failed (port:%u)", conf.port);
        return -1;
    }
    _port = conf.port;
    _pThreadCtl = pThreadCtl;
    _bStop = false;
    return 0;
}

int ListenThread::Stop()
{
    if (_bStop) return 0;
    _bStop = true;

    _OutputQueue.Stop();
    if (_pEvThread != nullptr)
    {
        _pEvThread->Stop();
    }
    if (_pListener != nullptr)
    {
        _pListener->Disable();
        _pListener->Free();
        SAFE_DELETE(_pListener);
    }
    return 0;
}

MsgQueue& ListenThread::OutPut()
{
    return _OutputQueue;
}

int ListenThread::Fini()
{
    _bStop = true;
    if (_pListener != nullptr)
    {
        _pListener->Free();
        SAFE_DELETE(_pListener);
    }
    _port = 0;
    _pThreadCtl = nullptr;
    if (_pEvThread != nullptr)
        delete _pEvThread;
    _pEvThread = nullptr;
    return 0;
}

int ListenThread::Start()
{
    if (_pEvThread != nullptr)
        return _pEvThread->Start(&_ListenThreadFunc, (void*) this);
    return -1;
}

void ListenThread::_OnMsgOutFun(const event_msg_t msg)
{
    HostHd* pHostMsgHd = (HostHd*) msg;
    if (pHostMsgHd->type == EHostMtType::CONN)
    {
        ConnMsg* pConnMsg = (ConnMsg*) msg;
        if (pConnMsg->body.state == EConnState::NET_EXIT)
        {
            Stop();
        }
    }
}

//////////////////////////////////////////////////////////////////////////

