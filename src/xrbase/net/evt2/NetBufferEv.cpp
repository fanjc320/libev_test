/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "NetBufferEv.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "NetEventBase.h"
#include "log/LogMgr.h"

#ifndef _MSC_VER
#include <linux/tcp.h>
#endif

NetBufferEv* net_socket_new(NetEventBase* base, SOCKET_T fd, int options)
{

    bufferevent* p = bufferevent_socket_new(base->EventBase(), fd, options);
    if (p == nullptr)
    {
        LOG_ERROR("error");
        return nullptr;
    }
    NetBufferEv* pEv = new NetBufferEv;
    pEv->SetBufEv(p);
    return pEv;
}

NetBufferEv* net_socket_new(NetEventBase* base, SOCKET_T fd, bool bCloseOnFree)
{
    int opts = 0;
    if (bCloseOnFree)
    {
        opts = BEV_OPT_CLOSE_ON_FREE;
    }
    return net_socket_new(base, fd, opts);
}

void net_bufferev_free(NetBufferEv* pBuffEv)
{
    if (pBuffEv != nullptr)
    {
        pBuffEv->Release();
        delete pBuffEv;
    }
}

int net_socketpair(int d, int type, int protocol, SOCKET_T sv[2])
{
    return evutil_socketpair(d, type, protocol, sv);
}

bool net_is_connected_events(short events)
{
    return (events & BEV_EVENT_CONNECTED) != 0;
}

void NetBufferEv::Release()
{
    if (_bufev != nullptr)
    {
        bufferevent_free(_bufev);
        _bufev = nullptr;
    }
    _EvBuffIn.Release();
    _EvBuffOut.Release();
}

size_t NetBufferEv::Read(void* data, size_t size)
{
    LOG_MINE("mine", "fd:%d", GetFd());
    return bufferevent_read(_bufev, data, size);
}

int NetBufferEv::Write(const void* data, size_t size)
{
    LOG_MINE("mine", "fd:%d", GetFd());
    return bufferevent_write(_bufev, data, size);
}

NetEvBuffer* NetBufferEv::GetInput()
{
    evbuffer* pBuf = bufferevent_get_input(_bufev);
    _EvBuffIn.SetEvBuff(pBuf);
    return &_EvBuffIn;
}

NetEvBuffer* NetBufferEv::GetOut()
{
   evbuffer* pBuf = bufferevent_get_output(_bufev);
   _EvBuffOut.SetEvBuff(pBuf);
   return &_EvBuffOut;
}

void NetBufferEv::SetCb(FUN_BUFF_DATA_CB readcb, FUN_BUFF_DATA_CB writecb, FUN_BUFF_EVENT_CB eventcb, void* cbarg)
{
    bufferevent_setcb(_bufev, readcb, writecb, eventcb, cbarg);
}

int NetBufferEv::Enable(short event)
{
    return bufferevent_enable(_bufev, event);
}

void NetBufferEv::SetWaterMark(short events, size_t lowmark, size_t highmark)
{
    bufferevent_setwatermark(_bufev, events, lowmark, highmark);
}

SOCKET_T NetBufferEv::GetFd()
{
    return bufferevent_getfd(_bufev);
}

int NetBufferEv::Connect(const struct sockaddr* addr, int socklen)
{
    return bufferevent_socket_connect(_bufev, addr, socklen);
}

void NetBufferEv::SetBufEv(bufferevent* pEv)
{
    _bufev = pEv;
}

bufferevent* NetBufferEv::BufEv()
{
    return _bufev;
}

size_t NetEvBuffer::GetLength()
{
    return evbuffer_get_length(_evBuff);
}

size_t NetEvBuffer::CopyOut(void* data_out, size_t datlen)
{
    return evbuffer_copyout(_evBuff, data_out, datlen);
}

void NetEvBuffer::SetEvBuff(evbuffer* pEvBuff)
{
    _evBuff = pEvBuff;
}

evbuffer* NetEvBuffer::GetEvBuff()
{
    return _evBuff;
}

void NetEvBuffer::Release()
{
    _evBuff = nullptr;
}
