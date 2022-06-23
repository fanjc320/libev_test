/*
 *
 *      Author: venture
 */
#pragma once

#include "net/NetSetting.h"
#include "net/NetBase.h"
#include "net/EventTimer.h"

class NetBufferEv;
class IoThread;
class NetConn;
class NetConnPool;

class NetConn
{
public:
    NetConn(conn_oid_t iConnId, NetConnPool* pConnPool);
    ~NetConn();

    int Init(SOCKET_T fd, NetEventBase* pEvBase, sockaddr& addr, EConnType type
        , uint32_t nSecRecvLimit);
    int Release();

    NetBufferEv* GetEvBuffer();
    conn_oid_t GetConnId();
    thread_oid_t GetThreadId();
    NetConnPool* GetPool();
    IoThread* GetThread();
    uint32_t GetIp();
    EConnType GetType();
    bool IsVaild() { return _type != EConnType::UNKNOWN; }

    uint32_t GetMaxBuffer();
    void SetMaxBuffer(uint32_t nMaxBuffer);

    bool AddRecvSec();
    time_t GetLastRecv();
    uint32_t GetSecRecv();
    uint32_t GetAllRecv();

    EParseResult GetParseCode() const;
    void SetParseCode(EParseResult val);

    int Write(const void* pData, size_t size);
private:
    conn_oid_t  _iConnId = INVALID_CONN_OID;
    EConnType   _type = EConnType::UNKNOWN;

    NetConnPool* _pConnPool = nullptr;
    NetBufferEv* _pBuff = nullptr;
    uint32_t     _nMaxBuffSize = DEF_MAX_BUFFER;
    time_t      _tmLast = 0;
    uint32_t    _nSecRecvs = 0;
    uint32_t    _nSecRecvLimit = 0;
    uint32_t    _nAllRecv = 0;
    SOCKET_T    _socket = -1;
    EParseResult   _eParseCode = EParseResult::OK;

    uint32_t    _ip = 0;
    sockaddr    _addr;
};
