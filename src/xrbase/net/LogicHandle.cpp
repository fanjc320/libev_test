/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "LogicHandle.h"
#include "base/MsgConn.h"
#include "base/NetConn.h"
#include "base/IoThreadControl.h"
#include "base/IoThread.h"
#include "net/base/sock_opt.h"
#include "log/LogMgr.h"
#include "time/time_func_x.h"
#include "string/str_format.h"
#include "base/ReConnEvent.h"
#include "net/MsgParser.h"
#include "net/NetBase.h"
#include "base/IoThreadControl.h"

LogicSession::LogicSession()
    : _soid(INVALID_SESSION_OID)
    , _state(SS_UNKNOWN)
    , _ip(0)
{
    _hd.threadOid = INVALID_THREAD_OID;
    _hd.connOid = INVALID_CONN_OID;
    memset(_strip, 0, sizeof(_strip));
}
LogicSession::~LogicSession()
{

}

void LogicSession::OnAccpeted()
{
    _state = SS_CONN_REG_OK;
    OnAccept();
}
void LogicSession::OnConnected()
{
    _state = SS_CONN_REG_OK;
    OnConnect();
}
void LogicSession::OnDisconnected()
{
    _tmDiscon = get_curr_time();
    _state = SS_CONN_DISCONN;
    OnDisconnect();
}

bool LogicSession::SendNet(NetMsg* pMsg)
{
    _tmSend = get_curr_time();
    return send_conn_msg(_pIoThreadCtl, _hd.threadOid, _hd.connOid, pMsg);
}

bool LogicSession::SendNet(const MsgHead* pMsg)
{
    _tmSend = get_curr_time();
    return send_conn_msg_x(_pIoThreadCtl, _hd.threadOid, _hd.connOid, pMsg);
}

bool LogicSession::IsConnected()
{
    return _state == SS_CONN_REG_OK;
}

void LogicSession::SetIp(uint32_t ip)
{
    _ip = ip;
    std::string strIp = sock_opt::Inet_Ip(ip);
    strncpy_safe(_strip, strIp);
}

void LogicSession::SendPing(int64_t nInterval)
{
    if (get_curr_time() - _tmSend < nInterval)
        return;

    MsgHead head;
    head.flag = HEAD_FLAG_SVR_PING;
    head.mod_id = UINT8_MAX;
    head.proto_id = UINT8_MAX;
    head.code = 0;
    head.seq_id = 0;
    head.size = MSG_HEAD_SIZE;
    SendNet(&head);
}

//////////////////////////////////////////////////////////////////////////
ReconnSession::ReconnSession()
    : _pReconn(nullptr)
{
    _type = EConnType::RECONN;
}
ReconnSession::~ReconnSession()
{
    if (_pReconn != nullptr)
        delete _pReconn;
    _pReconn = nullptr;
}
void ReconnSession::OnDisconnect()
{
    _pReconn->OnDisconnect();
    LogicSession::OnDisconnect();
}

void ReconnSession::Stop()
{
    if (_pReconn != nullptr)
        _pReconn->StopTimer();
}

const char* ReconnSession::GetIpStr()
{
    return _pReconn->GetIp().c_str();
}

uint16_t ReconnSession::GetPort()
{
    return _pReconn->GetPort();
}

void ReconnSession::SetReconnEvent(ReconnEvent* pReconn)
{
    pReconn->SetOwner(this);
    _pReconn = pReconn;
}

//////////////////////////////////////////////////////////////////////////
LogicHandle::LogicHandle()
    : _pIoThreadCtl(nullptr)
{ }

LogicHandle::~LogicHandle()
{

}
void LogicHandle::Initialize(IoThreadControl* pThreadCtl, thread_oid_t tNum, conn_oid_t conNum)
{
    _pIoThreadCtl = pThreadCtl;
    _sessVecs.resize(tNum);
    for (int i = 0; i < (int) _sessVecs.size(); ++i)
    {
        _sessVecs[i].resize(conNum);
    }
}

void LogicHandle::OnHandleConnMsg(const void* msg)
{
    ConnMsg* pConnMsg = (ConnMsg*) msg;
    const ConnMsgBody& body = pConnMsg->body;
    const HostHd& hd = pConnMsg->hd;
    LogicSession* pSession = nullptr;
    
    switch (body.state)
    {
        case EConnState::NET_CONNECTED:
            LOG_MINE("mine", "", "");

            pSession = GetReconSession(body.soid);
            if (pSession)
            {
                AddSession(pSession, hd, 0);
                pSession->OnConnected();
            }
            break;
        case EConnState::NET_DISCONN:
            pSession = GetSession(hd);
            if (pSession)
            {
                _RemoveSession(pSession);
                SendIoConnMsg(hd, EConnState::LAYER_DISCONN, pSession->GetSoid(), 0);
                pSession->OnDisconnected();
            }
            else
                SendIoConnMsg(hd, EConnState::LAYER_DISCONN, -1, 0);
            break;
        case EConnState::NET_NORECV:
            pSession = GetSession(hd);
            if (pSession)
            {
                pSession->SetTimeOut(true);
                _RemoveSession(pSession);
                //send_io_conn_msg(hd, cs_layer_disconn, pSession->get_id(), 0);
                pSession->OnDisconnected();
            }
            break;
        case EConnState::NET_ACCEPTED:
            pSession = NewServerSession();
            if (pSession)
            {
                LOG_MINE("mine", "", "");

                AddSession(pSession, hd, 0);
                pSession->OnPrepared();
            }
            break;
        default:
            break;
    }
}

bool LogicHandle::OnHandleLogicMsg(const NetMsg* pNetMsg)
{
    MsgHead* pHead = MSG_HEAD_FROM_NET(pNetMsg);
    if (pHead->flag == HEAD_FLAG_SVR_PING_RET)
    {
        return true;
    }  
	if (pHead->flag == HEAD_FLAG_SVR_PING)
	{
		pHead->flag = HEAD_FLAG_SVR_PING_RET;
		send_conn_msg_x(_pIoThreadCtl, pNetMsg->hd.threadOid, pNetMsg->hd.connOid, pHead);
		return true;
	}
    LogicSession* pSession = GetSession(pNetMsg->hd);
    if (pSession)
    {
        pSession->OnHandleMsgV(pNetMsg);
        return true;
    }
    return false;
}

void LogicHandle::Stop()
{
    for (auto itr = _mapReconSes.begin(); itr != _mapReconSes.end(); ++itr)
    {
        LogicSession* pSession = itr->second;
        ReconnSession* pRecon = dynamic_cast<ReconnSession*>(pSession);
        if (pRecon != nullptr)
            pRecon->Stop();
    }
}

session_oid_t LogicHandle::AddReconSession(ReconnSession* pSession)
{
    pSession->SetIoThreadCtl(_pIoThreadCtl);
    pSession->SetSoid(++_nReconNum);
    _mapReconSes[pSession->GetSoid()] = pSession;
    return _nReconNum;
}

ReconnSession* LogicHandle::GetReconSession(session_oid_t id)
{
    auto itr = _mapReconSes.find(id);
    if (itr != _mapReconSes.end())
        return itr->second;

    return nullptr;
}

int LogicHandle::KillReconSession(ReconnSession* pSession)
{
    SendIoConnMsg(pSession->GetHd(), EConnState::LAYER_KILL, pSession->GetSoid(), 0);
    _RemoveSession(pSession);
    pSession->OnDisconnect();
    _mapReconSes.erase(pSession->GetSoid());

    return 0;
}

int LogicHandle::AddSession(LogicSession* pSession, const HostHd& hd, uint32_t maxBuffer)
{
    pSession->SetIoThreadCtl(_pIoThreadCtl);
    if (!_HasSession(hd))
    {
        IoThread* ioThread = _pIoThreadCtl->GetIoThread(hd.threadOid);
        if (ioThread)
        {
            NetConnPool* connPool = ioThread->ConnPool();
            if (connPool)
            {
                int ip = connPool->GetIp(hd.connOid);
                pSession->SetIp(ip);
            }
        }

        LOG_MINE("mine", "", "");

        pSession->SetHd(hd);
        _sessVecs[hd.threadOid][hd.connOid] = pSession;

        if (maxBuffer > 0)
        {
            SendIoConnMsg(hd, EConnState::LAYER_CONNECTED, pSession->GetSoid(), maxBuffer);
        }

        if (pSession->Type() == EConnType::ACCEPTED)
        {
            pSession->OnAccpeted();
            pSession->SetSoid(++_nSessionNum);
        }

        return 0;
    }
    else
    {
        LOG_ERROR("hd con:%d", hd.connOid);
    }
    return -1;
}

int LogicHandle::_RemoveSession(LogicSession* pSession)
{
    const HostHd& hd = pSession->GetHd();
    if (_HasSession(hd))
    {
        if (pSession == _sessVecs[hd.threadOid][hd.connOid])
        {
            _sessVecs[hd.threadOid][hd.connOid] = nullptr;
            return 0;
        }
    }
    return -1;
}

int LogicHandle::KillSession(LogicSession* pSession)
{
    SendIoConnMsg(pSession->GetHd(), EConnState::LAYER_KILL, pSession->GetSoid(), 0);
    _RemoveSession(pSession);
    pSession->OnDisconnect();
    return 0;
}

LogicSession* LogicHandle::GetSession(const HostHd& hd)
{
    if (hd.threadOid >= 0 && hd.threadOid < (int) _sessVecs.size())
    {
        SessionVec& sessionVec = _sessVecs[hd.threadOid];
        if (hd.connOid >= 0 && hd.connOid < (int) sessionVec.size())
        {
            return sessionVec[hd.connOid];
        }
    }
    return nullptr;
}

bool LogicHandle::SendConnMsg(NetMsg* pMsg, const HostHd& hd)
{
    if (_pIoThreadCtl == nullptr) return false;
    return send_conn_msg(_pIoThreadCtl, hd.threadOid, hd.connOid, pMsg);
}

bool LogicHandle::SendConnMsg(const MsgHead* pMsg, const HostHd& hd)
{
    if (_pIoThreadCtl == nullptr) return false;
    return send_conn_msg_x(_pIoThreadCtl, hd.threadOid, hd.connOid, pMsg);
}

bool LogicHandle::_HasSession(const HostHd& hd)
{
    return GetSession(hd) != nullptr;
}

int LogicHandle::SendIoConnMsg(const HostHd& hd, EConnState state, session_oid_t soid, uint32_t maxBuffer)
{
    if (_pIoThreadCtl)
    {
        return logic_notify_io_conn_msg(_pIoThreadCtl, hd, state, soid, maxBuffer);
    }
    return -1;
}
