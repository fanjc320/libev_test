/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <errno.h>
#include <functional>

#include "NetConnPool.h"
#include "IoThread.h"
#include "NetConn.h"
#include "ListenThread.h"
#include "container/fix_id_alloc.h"
#include "MsgConn.h"
#include "log/LogMgr.h"
#include "EventThread.h"
#include "IoThreadControl.h"
#include "net/base/sock_opt.h"
#include "net/MsgParser.h"
#include "time/time_func_x.h"

//////////////////////////////////////////////////////////////////////////

NetConnPool::NetConnPool()
{

}

NetConnPool::~NetConnPool()
{
    Release();
}


bool NetConnPool::Init(IoThread* pThread, uint16_t nMaxConn)
{
    if (nMaxConn == 0)
    {
        return false;
    }

    _pConns = new NetConn * [nMaxConn];
    memset(_pConns, 0, sizeof(_pConns[0]) * nMaxConn);
    _nMaxConns = nMaxConn;

    if (!(_idAlloc.init(nMaxConn)))
    {
        return false;
    }

    _pIoThread = pThread;

    return true;
}

int NetConnPool::Release()
{
    _tvdetect.Stop();
    _mapkill.clear();

    if (_pConns != nullptr)
    {
        for (conn_oid_t id = 0; id < _nMaxConns; ++id)
        {
            DelConn(id);
        }
        delete[] _pConns;
        _pConns = nullptr;
    }
    _idAlloc.release();
    _setUserID.clear();
    return 0;
}

IoThread* NetConnPool::IoThreads()
{
    return _pIoThread;
}

inline bool NetConnPool::IsVaild(conn_oid_t id)
{
    return id >= 0 && id < _nMaxConns;
}

bool NetConnPool::StartTimer(int nDetectSec)
{
    bool res = false;
    _nDetectSec = nDetectSec;

    res = _tvdetect.Init(_pIoThread->EvThread()->Base(), true
        , CHECK_DETECT_INTERVAL * 1000
        , std::bind(&NetConnPool::_OnTimerDetect, this, std::placeholders::_1));

    return res;
}

NetConn* NetConnPool::FindConn(conn_oid_t id)
{
    if (IsVaild(id))
    {
        return _pConns[id];
    }
    return nullptr;
}

NetConn* NetConnPool::NewConn(uint32_t maxBuffer)
{
    if (_idAlloc.full())
    {
        remove_killed();
    }

    uint16_t id = _idAlloc.alloc();
    if (!IsVaild(id))
    {
        return nullptr;
    }
    NetConn* pConn = nullptr;
    pConn = _pConns[id];
    if (pConn == nullptr)
    {
        pConn = new NetConn(id, this);
        _pConns[id] = pConn;
    }
    else
        pConn = new (pConn)NetConn(id, this);

    if (maxBuffer > 0)
    {
        pConn->SetMaxBuffer(maxBuffer);
    }
    else
    {
        pConn->SetMaxBuffer(DEF_MAX_BUFFER);
    }
    _setUserID.insert(id);

    return pConn;
}

int NetConnPool::GetIp(conn_oid_t id)
{
    NetConn* con = FindConn(id);
    if (con != nullptr)
        return con->GetIp();
    return 0;
}

size_t NetConnPool::ConnSize()
{
    return _idAlloc.size();
}

bool NetConnPool::Full()
{
    return _idAlloc.full();
}

void NetConnPool::remove_killed()
{
    time_t tmNow = get_curr_time();
    conn_oid_t conid = INVALID_CONN_OID;

    for (auto& itr : _mapkill.map())
    {
        conid = itr.second->id;
        DelConn(conid);
    }
    _mapkill.clear();
}

int NetConnPool::DelConn(conn_oid_t id)
{
    if (!IsVaild(id))
    {
        return -1;
    }
    _setUserID.erase(id);
    _idAlloc.free(id);
    NetConn*& pConn = _pConns[id];
    if (pConn)
    {
        pConn->Release();
    }
    _mapkill.erase(id);
    return 0;
}

int NetConnPool::KillConn(conn_oid_t id)
{
    NetConn* pConn = FindConn(id);
    if (pConn == nullptr) return -1;

    pConn->SetParseCode( EParseResult::CUS_KILL);
    LOG_ERROR("kill  netid:%d", id);
    _mapkill.insert(id, con_time(id, get_curr_time()));
    return 0;
}

int NetConnPool::Write(conn_oid_t id, const void* pData, size_t size)
{
    NetConn* pConn = FindConn(id);
    if (pConn && pConn->IsVaild())
        return pConn->Write(pData, size);
    else
        return -1;
}

int NetConnPool::DisConn(conn_oid_t id)
{
    NetConn* pConn = FindConn(id);
    if (pConn)
    {
        return io_notify_logic_conn_msg(pConn, EConnState::NET_DISCONN, INVALID_SESSION_OID);
    }
    return -1;
}

void NetConnPool::_OnTimerDetect(uint64_t dwTM)
{
    _OnNetConnDetect(dwTM);
    _OnNetConnKill(dwTM);
}

void NetConnPool::_OnNetConnDetect(uint64_t dwTM)
{
    if (_pConns != nullptr)
    {
        NetConn* pConn = nullptr;
        time_t tmNow = get_curr_time();
        for (auto itr = _setUserID.begin(); itr != _setUserID.end();)
        {
            pConn = FindConn(*itr);
            if (pConn && pConn->GetLastRecv() != 0
                && abs(tmNow - pConn->GetLastRecv()) >= _nDetectSec)
            {
                itr = _setUserID.erase(itr);
                LOG_ERROR("detect:%ds kill net ConnId=%d, threadid:%d"
                    , _nDetectSec, pConn->GetConnId(), pConn->GetThreadId());
                io_notify_logic_conn_msg(pConn, EConnState::NET_NORECV, INVALID_SESSION_OID);
                DelConn(pConn->GetConnId());
            }
            else
                ++itr;
        }
    }
}

void NetConnPool::_OnNetConnKill(uint64_t dwTM)
{
    time_t tmNow = get_curr_time();
    conn_oid_t conid = INVALID_CONN_OID;
    for (auto itr = _mapkill.begin(); itr != _mapkill.end(); )
    {
        if (abs(itr->tmKill + 20) <= tmNow)
        {
            conid = itr->id;
            itr = _mapkill.erase(conid, itr);
            DelConn(conid);
        }
        else
            return;
    }
}
