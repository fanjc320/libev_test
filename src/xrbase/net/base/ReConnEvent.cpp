/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <typeinfo>

#ifndef _MSC_VER
#include <linux/tcp.h>
#endif

#include "ReConnEvent.h"
#include "ListenThread.h"
#include "net/LogicHandle.h"
#include "log/LogMgr.h"
#include "EventThread.h"
#include "IoThread.h"
#include "IoThreadControl.h"
#include "net/base/sock_opt.h"
#include "net/evt2/NetEventBase.h"
#include "net/evt2/NetBufferEv.h"
#include "NetEventCb.h"

ReconnEvent::ReconnEvent(EventThread* pThread,
    IoThreadControl* pThreadCtl, int iConnSecs, uint32_t maxBuffer)
    : _pOwner(nullptr)
    , _pBuffer(nullptr)
    , _pThread(pThread)
    , _pThreadCtl(pThreadCtl)
    , _nPort(0)
    , _bConnected(false)
    , _bConnecting(false)
    , _maxBuffer(maxBuffer)
{
    memset(&_addr, 0, sizeof(_addr));
    _tmrConnect.Init((int64_t) iConnSecs * 1000);
    _tmrPing.Init(5);
}

ReconnEvent::~ReconnEvent()
{
    _FreeBufferEvent();
}

void ReconnEvent::Connect()
{
    if (_bConnected || _bConnecting)
    {
        return;
    }

    if (_pOwner == nullptr)
    {
        LOG_ERROR("Owner is null!");
        return;
    }
    if (_pOwner != nullptr && _objName.empty())
    {
        _objName = typeid(*_pOwner).name();
    }

    ++_nConnectAll;
    ++_nConnectNum;

    if (_nConnectNum % 10 == 1)
        LOG_WARNING("%s connect to (%s:%u), times:%u...", _objName.c_str()
            , _strIp.c_str(), (uint32_t) _nPort, _nConnectNum);

    memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = inet_addr(_strIp.c_str());
    _addr.sin_port = htons(_nPort);
    _pOwner->SetIp(_addr.sin_addr.s_addr);

    if (_pBuffer == nullptr)
    {
        _pBuffer = net_socket_new(_pThread->Base(), -1, true);//唯二调用net_socket_new的地方,另:NetConn::init
    }
    if (_pBuffer == nullptr)
    {
        LOG_ERROR("buffer new error! %s", _objName.c_str());
        return;
    }

    _pBuffer->SetCb(nullptr, nullptr, reconnect_cb, (void*) this);
    if (_pBuffer->Connect((sockaddr*) &_addr, sizeof(_addr)) < 0)
    {
        _FreeBufferEvent();
        LOG_ERROR("buffer connect error! %s", _objName.c_str());
        return;
    }
    sock_opt::SetCloseExec(_pBuffer->GetFd());
    if (!sock_opt::SetNoBlock(_pBuffer->GetFd()))
    {
        LOG_ERROR(" nonblocking  client: %d error:%d, %s", _pBuffer->GetFd(), errno
            , strerror(errno));
    }
    _bConnecting = true;
}

void ReconnEvent::OnConnect(short events)
{
    _bConnecting = false;
    _bConnected = net_is_connected_events(events);

    if (_bConnected)
    {
        LOG_INFO("%s connect to (%s:%u)(no=%d) times=%u alltimes=%u ok", _objName.c_str(),
            _strIp.c_str(), (uint32_t) _nPort, events, _nConnectNum, _nConnectAll);
        _nConnectNum = 0;
    }
    else
    {
        if (_nConnectNum % 10 == 1)
        {
            LOG_WARNING("%s connect to (%s:%u)(no=%d) times:%u alltimes=%u failed", _objName.c_str()
                , _strIp.c_str(), (uint32_t) _nPort, events, _nConnectNum, _nConnectAll);
        }
    }

    if (_bConnected)
    {
        SOCKET_T fd = _pBuffer->GetFd();
        LOG_MINE("mine", "fd:%d", fd);
        if (fd != -1 && _pOwner != nullptr)
        {
            _pThreadCtl->DispatchConnect(fd, _pOwner->GetSoid()
                , *(sockaddr*) (&_addr), _maxBuffer);
        }
        else
        {
            _bConnected = false;
            LOG_ERROR("%s connect to (%s:%u)(no=%d) times:%u alltimes=%u failed", _objName.c_str()
                , _strIp.c_str(), (uint32_t) _nPort, events, _nConnectNum, _nConnectAll);
        }
    }
    else
    {
        _FreeBufferEvent();
    }
}

void ReconnEvent::OnDisconnect()
{
    _bConnected = false;
    _bConnecting = false;

    _FreeBufferEvent();
}

void ReconnEvent::StopTimer()
{
    _bTimerStop = true;
}

void ReconnEvent::OnTimer(uint64_t dwTM)
{
    if (_bTimerStop)
        return;

    if (_tmrPing.On() && _bConnected)
    {
        if(_pOwner) _pOwner->SendPing(_tmrPing.InterVal());
    }

    if (!_tmrConnect.On(dwTM))
        return;

    if (!_bConnected && !_bConnecting)
    {
        if(_pOwner) _pOwner->OnConnectTimes(_nConnectNum);
        Connect();
    }
}

void ReconnEvent::Init(const std::string& strIp, uint16_t nPort)
{
    _strIp = strIp;
    _nPort = nPort;

    Connect();
}

void ReconnEvent::_FreeBufferEvent()
{
    net_bufferev_free(_pBuffer);
    _pBuffer = nullptr;
}
