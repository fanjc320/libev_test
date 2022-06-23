/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <signal.h>
#include <thread>
#include <chrono>
#include "LogicThread.h"
#include "base/ListenThread.h"
#include "base/ReConnEvent.h"
#include "net/LogicHandle.h"
#include "log/LogMgr.h"
#include "base/EventThread.h"
#include "base/IoThreadControl.h"
#include "base/IoThread.h"
#include "platform/func_proc.h"
#include "base/MsgConn.h"

LogicThread::LogicThread()
{ }

LogicThread::~LogicThread()
{
    if (_pIoThreadCtl != nullptr)
        delete _pIoThreadCtl;
    if (_pReConnThreadCtl != nullptr)
        delete _pReConnThreadCtl;
    if (_pListenThread != nullptr)
        delete _pListenThread;
    if (_pEvThread != nullptr)
        delete _pEvThread;

    if (_pTimercon != nullptr)
        delete _pTimercon;

    _pListenThread = nullptr;
    _pEvThread = nullptr;
    _pIoThreadCtl = nullptr;
    _pReConnThreadCtl = nullptr;
    _pTimercon = nullptr;
}

bool LogicThread::_CreateListener(IoThreadControl* pIoThreadCtl, LogicThread* pThread, const NetSetting& conf)
{
    _pListenThread = new ListenThread;
    _pListenThread->name = this->name + "_listen";
    if (_pListenThread->Init(pThread->EvThread(), conf, pIoThreadCtl) != 0)
    {
        LOG_ERROR("listen_thread_init failed (port:%u)", conf.port);
        return false;
    }

    if (_pListenThread->Start() != 0)
    {
        LOG_ERROR("start_libevent_thread failed");
        return false;
    }

    LOG_WARNING("listen (port:%u) ... sucess", conf.port);
    return true;
}

bool LogicThread::_CreateThreadCtl(IoThreadControl* pIoThreadCtl, LogicThread* pThread,
   const NetSetting& conf)
{
    if (pIoThreadCtl->Init(pThread->EvThread(), conf) != 0)
    {
        LOG_ERROR("init failed");
        return false;
    }
    if (pIoThreadCtl->Start() != 0)
    {
        LOG_ERROR("start failed");
        return false;
    }
    return true;
}

static bool has_listen(const NetSetting& setting)
{
    return  setting.port > 0;
}

LogicThread* LogicThread::New(const NetSetting& setting, const NetSetting& recon_setting)
{
    LogicThread* pLogicThread = LogicThread::New();
    if (pLogicThread == nullptr)
    {
        return nullptr;
    }

    bool bRet = pLogicThread->InitNetSetting(setting);
    ERROR_LOG_EXIT0(bRet);
    bRet = pLogicThread->InitNetReconnSetting(recon_setting);
    ERROR_LOG_EXIT0(bRet);
    return pLogicThread;

Exit0:
    Delete(pLogicThread);
    return nullptr;
}

LogicThread* LogicThread::New()
{
#ifdef _MSC_VER
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != 0)
    {
        LOG_ERROR("WSAStartup error ! res = %d", res);
    }
#endif

#ifndef _MSC_VER
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, 0);
#endif

    LogicThread* pLogicThread = new LogicThread;
    // main libevent_thread
    if (!pLogicThread->InitLogic())
    {
        delete pLogicThread;
        return nullptr;
    }
    return pLogicThread;
}

void LogicThread::Delete(LogicThread* pThread)
{
    if (pThread == nullptr) return;

    delete pThread;
    pThread = nullptr;

#ifdef _MSC_VER
    WSACleanup();
#endif
}

bool LogicThread::InitLogic()
{
    _pTimercon = new EventTimer;
    _pEvThread = new EventThread;
    // main libevent_thread
    if (_pEvThread->Init(0) != 0)
    {
        return false;
    }
    return true;
}

int LogicThread::MainLoop()
{
    return _pEvThread->MainLoop();
}

int LogicThread::Stop()
{
    if (_pTimercon != nullptr)
        _pTimercon->Stop();

    if (_pListenThread != nullptr) logic_notify_listen_conn_msg(_pListenThread, EConnState::NET_EXIT);
    if (_pIoThreadCtl != nullptr) logic_notify_io_conn_msg(_pIoThreadCtl, EConnState::NET_EXIT);
    if (_pReConnThreadCtl != nullptr) logic_notify_io_conn_msg(_pReConnThreadCtl, EConnState::NET_EXIT);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    if (_pListenThread != nullptr)   _pListenThread->Stop();
    if (_pIoThreadCtl != nullptr)  _pIoThreadCtl->Stop();
    if (_pReConnThreadCtl != nullptr)  _pReConnThreadCtl->Stop();
    if (_pEvThread != nullptr) _pEvThread->Stop();

    return 0;
}

EventThread* LogicThread::EvThread()
{
    return _pEvThread;
}

bool LogicThread::InitNetSetting(const NetSetting& setting)
{
    if (has_listen(setting))
    {
        ERROR_LOG_EXIT0(setting.IsValid());
        if (is_listen_port(setting.port))
        {
            LOG_ERROR("listen port exsit %u!", setting.port);
            goto Exit0;
        }

        _pIoThreadCtl = new IoThreadControl;
        _pIoThreadCtl->name = this->name+"_netsetting-ioctrl";
        // io thread dispatcher
        ERROR_LOG_EXIT0(_CreateThreadCtl(_pIoThreadCtl, this, setting) && 
            _CreateListener(_pIoThreadCtl, this, setting));
    }

    _setting = setting;
    return true;
Exit0:
    return false;
}

bool LogicThread::InitNetReconnSetting(const NetSetting& recon_setting)
{
    // reconn io thread dispatcher
    if (recon_setting.IsValid())
    {
        _pReConnThreadCtl = new IoThreadControl;
        _pReConnThreadCtl->name = this->name + "_netreconn-ioctrl";
        ERROR_LOG_EXIT0(_CreateThreadCtl(_pReConnThreadCtl, this, recon_setting));

        ERROR_LOG_EXIT0(_pTimercon->Init(EvThread()->Base(), true
            , (uint64_t) recon_setting.reconSec * 1000
            , std::bind(&LogicThread::OnTimer, this, std::placeholders::_1)));
    }
    _reconSetting = recon_setting;
    return true;
Exit0:
    return false;
}

//////////////////////////////////////////////////////////////////////////

int LogicThread::AddReconn(ReconnSession* pSession, const std::string& strIp, uint16_t nPort, int iConnSecs, uint32_t maxBuffer)
{
    LogicHandle* pLogic = _pReConnThreadCtl->GetLogicHandle();
    if (pLogic)
    {
        pLogic->AddReconSession(pSession);
        ReconnEvent* pReconn = new ReconnEvent(_pEvThread
            , _pReConnThreadCtl, iConnSecs, maxBuffer);
        if (pReconn)
        {
            pSession->SetReconnEvent(pReconn);
            pReconn->Init(strIp, nPort);
            return 0;
        }
    }
    else
    {
        LOG_ERROR("ReConnThreadCtl null!");
    }
    return -1;
}

int LogicThread::RemoveReconn(ReconnSession* pSession)
{
    LogicHandle* pLogicHandle = _pReConnThreadCtl->GetLogicHandle();
    if (pLogicHandle)
    {
        return pLogicHandle->KillReconSession(pSession);
    }
    return -1;
}

bool LogicThread::SendListen(NetMsg* pNetMsg)
{
    return _pIoThreadCtl->SendMsgHton(pNetMsg);
}

bool LogicThread::SendReconn(NetMsg* pNetMsg)
{
    return _pReConnThreadCtl->SendMsgHton(pNetMsg);
}

void LogicThread::RegInterfaceListen(LogicHandle* pLogicHandle)
{
    if (_pIoThreadCtl != nullptr)
        _pIoThreadCtl->RegLogicHandle(pLogicHandle);
    else
        LOG_ERROR("control null!");
}

void LogicThread::RegInterfaceReconn(LogicHandle* pLogicHandle)
{
    if (_pReConnThreadCtl != nullptr)
        _pReConnThreadCtl->RegLogicHandle(pLogicHandle);
    else
        LOG_ERROR("pReConn control null!");
}

void LogicThread::OnTimer(uint64_t dwTM)
{
    if (_pReConnThreadCtl != nullptr && _pReConnThreadCtl->GetLogicHandle() != nullptr)
    {
        auto mapPre = _pReConnThreadCtl->GetLogicHandle()->GetPreSessions();
        for (auto item = mapPre.begin(); item != mapPre.end(); ++item)
        {
            item->second->GetReconnEvent()->OnTimer(dwTM);
        }
    }
}

LogicHandle* LogicThread::GetLogicHandle()
{
    if (_pIoThreadCtl == nullptr) return nullptr;

    return _pIoThreadCtl->GetInterFace();
}
LogicHandle* LogicThread::GetReConnLogicHandle()
{
    if (_pReConnThreadCtl == nullptr) return nullptr;

    return _pReConnThreadCtl->GetInterFace();
}

IoThreadControl* LogicThread::GetThreadCtl()
{
    return _pIoThreadCtl;
}

IoThreadControl* LogicThread::GetReconnThreadCtl()
{
    return _pReConnThreadCtl;
}
