/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <errno.h>
#include <functional>

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
#include "NetConnPool.h"
#include "net/evt2/NetEventBase.h"
#include "net/evt2/NetBufferEv.h"
#include "NetEventCb.h"

NetConn::NetConn(conn_oid_t iConnId, NetConnPool* pConnPool)
{
    _iConnId = iConnId;
    _pConnPool = pConnPool;
    memset(&_addr, 0, sizeof(_addr));
}

NetConn::~NetConn()
{
    Release();
}

int NetConn::Init(SOCKET_T fd, NetEventBase* pEvBase, sockaddr& addr, EConnType type
    , uint32_t nSecRecvLimit)
{
    if (!sock_opt::SetNoBlock(fd))
    {
        LOG_ERROR(" nonblocking  client: %d error:%d, %s", fd, errno, strerror(errno));
    }

    if (!sock_opt::SetKeepALive(fd) || !sock_opt::SetBufSize(fd, 128 * 1024))
    {
        LOG_ERROR(" no_delay  client: %d error:%d, %s", fd, errno, strerror(errno));
        return -1;
    }
    sock_opt::SetCloseExec(fd);

    LOG_MINE("mine"," socknew fd:%d", fd);
    NetBufferEv* pBuffer = net_socket_new(pEvBase, fd, true);//Î¨¶þµ÷ÓÃnet_socket_new,Áí£ºReConnEvent::Connect
    if (pBuffer == nullptr) return -1;

    pBuffer->SetCb(conn_read_cb, conn_write_cb, conn_event_cb, this);
    init_buffer_mask(pBuffer);

    _pBuff = pBuffer;
    _addr = addr;
    _ip = ((sockaddr_in*) (&addr))->sin_addr.s_addr;
    _type = type;
    _nSecRecvLimit = nSecRecvLimit;
    LOG_MINE("mine", " socknew fd:%d", fd);
    _socket = fd;
    _tmLast = get_curr_time();
    return 0;
}

int NetConn::Release()
{
    _iConnId = INVALID_CONN_OID;
    _type = EConnType::UNKNOWN;
    _nMaxBuffSize = DEF_MAX_BUFFER;

    _ip = 0;
    _tmLast = 0;
    _nSecRecvs = 0;
    _nAllRecv = 0;
    _eParseCode = EParseResult::OK;

    _socket = -1;
    if (_pBuff != nullptr)
    {
        net_bufferev_free(_pBuff);
        _pBuff = nullptr;
    }

    return 0;
}

NetBufferEv* NetConn::GetEvBuffer()
{
    return _pBuff;
}

conn_oid_t NetConn::GetConnId()
{
    return _iConnId;
}

thread_oid_t NetConn::GetThreadId()
{
    return _pConnPool->IoThreads()->EvThread()->Id();
}

NetConnPool* NetConn::GetPool()
{
    return _pConnPool;
}

IoThread* NetConn::GetThread()
{
    return _pConnPool->IoThreads();
}

uint32_t NetConn::GetIp()
{
    return _ip;
}

EConnType NetConn::GetType()
{
    return _type;
}

time_t NetConn::GetLastRecv()
{
    return _tmLast;
}

uint32_t NetConn::GetSecRecv()
{
    return _nSecRecvs;
}

uint32_t NetConn::GetAllRecv()
{
    return _nAllRecv;
}

uint32_t NetConn::GetMaxBuffer()
{
    return _nMaxBuffSize;
}

void NetConn::SetMaxBuffer(uint32_t nMaxBuffer)
{
    _nMaxBuffSize = nMaxBuffer;
}

bool NetConn::AddRecvSec()
{
    time_t tmNow = get_curr_time();
    if (tmNow != _tmLast)
    {
        if (_nSecRecvs >= _nSecRecvLimit
            && _nSecRecvLimit != 0)
        {
            return false;
        }

        _nSecRecvs = 0;
        _tmLast = tmNow;
    }

    ++_nAllRecv;
    ++_nSecRecvs;
    return true;
}

EParseResult NetConn::GetParseCode() const
{
    return _eParseCode;
}
void NetConn::SetParseCode(EParseResult val)
{
    _eParseCode = val;
}

int NetConn::Write(const void* pData, size_t size)
{
    if (_pBuff == nullptr) return -1;

    NetEvBuffer* pOutBuf = _pBuff->GetOut();
    if (pOutBuf == nullptr) return -1;

    size_t outLength = pOutBuf->GetLength();
    if (outLength >= GetMaxBuffer())
    {
        LOG_ERROR("net_conn_write huge(conn_oid=%u), size:%d, maxsize:%d"
            , (uint32_t) GetConnId(), outLength, GetMaxBuffer());
        return -1;
    }
    return _pBuff->Write(pData, size);
}
