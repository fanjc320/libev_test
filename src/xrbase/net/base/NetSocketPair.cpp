/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "NetSocketPair.h"
#include "log/LogMgr.h"
#include "net/evt2/NetEventBase.h"
#include "net/evt2/NetEvent.h"
#include "net/evt2/NetBufferEv.h"
#include "net/NetBase.h"
#include "net/base/sock_opt.h"

NetSocketPair::NetSocketPair()
{

}

NetSocketPair::~NetSocketPair()
{
    Release();
}

int NetSocketPair::Write()
{
    if (!_bInit) return 0;

    int rc = 0;
    LOG_MINE("mine", "name:%s fd:%d",name.c_str(), _fdWrite);

#ifdef _MSC_VER   
    rc = ::send(_fdWrite, "", 1, 0);//唯一调用::send的地方
#else
    rc = ::send(_fdWrite, "", 1, MSG_DONTWAIT);
#endif

    if (rc < 0 || rc == EAGAIN || rc == EINTR)
    {
        LOG_ERROR("error!");
    }
    return rc;
}

void NetSocketPair::Read()
{
    if (!_bInit) return;

    static unsigned char buf[1024];
    LOG_MINE("mine", "name:%s fd:%d",name.c_str(), _fdRead);

#ifdef _MSC_VER
    while (::recv(_fdRead, (char*) buf, sizeof(buf), 0) >= 1024){ }//唯一调用::recv的地方
#else
    while (::recv(_fdRead, (char*) buf, sizeof(buf), MSG_DONTWAIT) >= 1024){ }
#endif
}

bool NetSocketPair::Init(NetEventBase* pEvBase, FUN_PAIR_CALL fn, void* args)
{
    SOCKET_T fds[2];
    if (net_socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1)
    {
        return false;
    }

    _fdWrite = fds[0];
    _fdRead = fds[1];
    _pEvBase = pEvBase;
    LOG_MINE("mine", "name:%s fdWrite:%d fdRead:%d",name.c_str(), _fdWrite, _fdRead);
    if (!sock_opt::SetNoBlock(_fdWrite) || !sock_opt::SetNoBlock(_fdRead))
    {
        LOG_ERROR("setsockopt noblock  client:%d, %d, err:%d, %s", _fdRead, _fdWrite, errno, strerror(errno));
        return false;
    }

    if (_pEvent == nullptr)
    {
        _pEvent = new NetEvent();
    }

    _pEvent->Assign(_pEvBase, _fdRead, NetEvent::EET_READ | NetEvent::EET_PERSIST, _EventPop, this);//唯一event_assign到pop的地方，另一个是timeout
    _pEvent->Add(nullptr);

    _bInit = true;
    _fnCall = fn;
    _args = args;

    return true;
}

bool NetSocketPair::Release()
{
    if (!_bInit) return true;

    if (_pEvBase == nullptr)
        return true;

    _pEvent->Del();
    SAFE_DELETE(_pEvent);

    if (_fdRead != -1)
    {
        sock_opt::CloseSocket(_fdRead);
        _fdRead = -1;
    }
    if (_fdWrite != -1)
    {
        sock_opt::CloseSocket(_fdWrite);
        _fdWrite = -1;
    }

    _pEvBase = nullptr;
    _bInit = false;

    return true;
}

void NetSocketPair::Stop()
{
    _bStop = true;
    Write();
}

bool NetSocketPair::IsStop()
{
    return _bStop;
}

void NetSocketPair::_EventPop(SOCKET_T fd, short events, void* args)
{
    NetSocketPair* pMsgQueue = (NetSocketPair*) args;
    if (pMsgQueue != nullptr)
    {
        LOG_MINE("mine", "fd:%d", fd);
        pMsgQueue->Read();
        pMsgQueue->_DoCall();
        if (pMsgQueue->IsStop())
            pMsgQueue->Release();
    }
}

void NetSocketPair::_DoCall()
{
    if (_fnCall != nullptr && _args != nullptr)
    {
        LOG_MINE("mine", "_fnCall","");
        _fnCall(_args);
    }
}