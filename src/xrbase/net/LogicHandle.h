/*
 *
 *      Author: venture
 */
#pragma once

#include <vector>
#include <list>
#include <string>
#include <atomic>

#include "dtype.h"
#include "net/NetBase.h"
#include "net/MsgHead.h"

class IoThreadControl;
enum ESessionState
{
    SS_UNKNOWN = 0,
    SS_CONN_REG_OK = 1,
    SS_CONN_REG_FAILED = 2,
    SS_CONN_DISCONN,
};

class LogicThread;
class LogicSession
{
public:
    LogicSession();
    virtual ~LogicSession();

    virtual void OnAccept() { }
    virtual void OnConnect() { }
    virtual void OnDisconnect() { }
    virtual void OnPrepared() { }
    virtual bool OnHandleMsgV(const NetMsg* pMsg) = 0;

    void SetIoThreadCtl(IoThreadControl* pThreadCtl) { _pIoThreadCtl = pThreadCtl; }
    IoThreadControl* GetIoThreadCtl() { return _pIoThreadCtl; }
    time_t GetDisconTime() { return _tmDiscon; }

    bool IsTimeOut() const { return _bTimeOut; }
    void SetTimeOut(bool val) { _bTimeOut = val; }

public:
    bool IsConnected();
    void SendPing(int64_t nInterval);

    void SetHd(const HostHd& hd) { _hd = hd; }
    const HostHd& GetHd() const { return _hd; }

    void SetSoid(session_oid_t id) { _soid = id; }
    session_oid_t GetSoid() const { return _soid; }

    EConnType Type() { return _type; }

    thread_oid_t GetToid() const { return _hd.threadOid; }
    conn_oid_t GetCoid() const { return _hd.connOid; }

    void SetIp(uint32_t ip);
    uint32_t GetIp() const { return _ip; }
    virtual const char* GetIpStr() { return _strip; }

    bool HasVaildHd() const
    {
        return _hd.threadOid != INVALID_THREAD_OID &&
            _hd.connOid != INVALID_CONN_OID;
    }

    virtual void OnAccpeted()final;
    virtual void OnConnected() final;
    virtual void OnDisconnected()final;

protected:
    virtual bool SendNet(NetMsg* pMsg) final;
    virtual bool SendNet(const MsgHead* pMsg) final;

protected:
    HostHd _hd;
    IoThreadControl* _pIoThreadCtl = nullptr;

    session_oid_t	_soid = 0;
    ESessionState	_state = SS_UNKNOWN;
    uint32_t		_ip = 0;
    char		    _strip[64];
    time_t          _tmDiscon = 0;
    EConnType       _type = EConnType::ACCEPTED;
    bool            _bTimeOut = false;
    time_t          _tmSend = 0;
};

class ReconnEvent;
class ReconnSession : public LogicSession
{
public:
    ReconnSession();
    virtual ~ReconnSession();

    virtual void SetReconnEvent(ReconnEvent* pReconn);
    ReconnEvent* GetReconnEvent() const { return _pReconn; }

    virtual void OnDisconnect();
    virtual void OnConnectTimes(uint32_t times) { }

    void Stop();

    virtual const char* GetIpStr() override;
    uint16_t GetPort();
private:
    ReconnEvent* _pReconn = nullptr;
};

class LogicHandle
{
public:
    LogicHandle();
    virtual ~LogicHandle();

    virtual LogicSession* NewServerSession() {return nullptr;}

    virtual bool OnHandleLogicMsg(const NetMsg* pNetMsg);

    void Stop();

public:
    void Initialize(IoThreadControl* pThreadCtl, thread_oid_t tNum, conn_oid_t conNum);
    void OnHandleConnMsg(const void* msg);
    int  AddSession(LogicSession* pSession, const HostHd& hd, uint32_t maxBuffer);
    int  KillSession(LogicSession* pSession);
    LogicSession* GetSession(const HostHd& hd);

    bool SendConnMsg(const class MsgHead* pMsg, const HostHd& hd);
    bool SendConnMsg(NetMsg* pMsg, const HostHd& hd);
    session_oid_t  AddReconSession(ReconnSession* pSession);
    ReconnSession* GetReconSession(session_oid_t id);
    int KillReconSession(ReconnSession* pSession);

    int SendIoConnMsg(const HostHd& hd, EConnState state, session_oid_t soid, uint32_t maxBuffer);
    umap<session_oid_t, ReconnSession*>& GetPreSessions() { return _mapReconSes; }

protected:
    bool _HasSession(const HostHd& hd);
    int  _RemoveSession(LogicSession* pSession);
    typedef std::vector<LogicSession*> SessionVec;
    typedef std::vector<SessionVec>    SessionVecList;

    IoThreadControl*        _pIoThreadCtl = nullptr;
    SessionVecList	        _sessVecs;

    //connect session
    std::atomic<session_oid_t> _nReconNum = 0;
    std::atomic<session_oid_t> _nSessionNum = 0;
    umap<session_oid_t, ReconnSession*> _mapReconSes;
};