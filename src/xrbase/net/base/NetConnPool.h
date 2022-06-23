/*
 *
 *      Author: venture
 */
#pragma once
#include <stdint.h>
#include "dtype.h"
#include "net/NetBase.h"
#include "net/EventTimer.h"
#include "container/fix_id_alloc.h"
#include "stl/map_fix.h"

class IoThread;
class NetConn;

class NetConnPool
{
public:
    NetConnPool();
    ~NetConnPool();

    bool Init(IoThread* pThread, uint16_t nMaxConn);

    int  Release();
    NetConn* FindConn(conn_oid_t id);
    NetConn* NewConn(uint32_t maxBuffer);

    int DelConn(conn_oid_t id);
    int KillConn(conn_oid_t id);

    int Write(conn_oid_t id, const void* pData, size_t size);
    int DisConn(conn_oid_t id);

    bool IsVaild(conn_oid_t id);
    bool StartTimer(int nDetectSec);

    IoThread* IoThreads();
    int       GetIp(conn_oid_t id);

    size_t ConnSize();
    bool Full();
    void remove_killed();

protected:
    struct con_time
    {
        conn_oid_t id;
        time_t     tmKill;
    };

    NetConn**   _pConns = nullptr;
    conn_oid_t  _nMaxConns = 5000;
    IoThread*   _pIoThread = nullptr;

    fix::fix_id_alloc<uint16_t, UINT16_MAX> _idAlloc;
    uset<uint16_t>    _setUserID;

    int _nDetectSec = CHECK_DETECT_INTERVAL;
    map_list<conn_oid_t, con_time> _mapkill;

    EventTimer  _tvdetect;  
    void _OnTimerDetect(uint64_t dwTM);
    void _OnNetConnDetect(uint64_t dwTM);
    void _OnNetConnKill(uint64_t dwTM);
};