/*
 *
 *      Author: venture
 */
#pragma once

#include <stdint.h>
#include "net/NetBase.h"

struct bufferevent;
struct evbuffer;

typedef void (*FUN_BUFF_DATA_CB)(struct bufferevent* bev, void* ctx);
typedef void (*FUN_BUFF_EVENT_CB)(struct bufferevent* bev, short what, void* ctx);

class  NetEventBase;
class NetEvBuffer
{
public:
    size_t GetLength();
    size_t CopyOut(void* data_out, size_t datlen);
    void SetEvBuff(evbuffer* pEvBuff);
    evbuffer* GetEvBuff();

    void Release();

private:
    evbuffer* _evBuff = nullptr;
};
class NetBufferEv
{ 
public:
    void Release();

    size_t Read(void* data, size_t size);
    int Write(const void* data, size_t size);

    NetEvBuffer* GetInput();
    NetEvBuffer* GetOut();

    void SetCb(FUN_BUFF_DATA_CB readcb, FUN_BUFF_DATA_CB writecb, FUN_BUFF_EVENT_CB eventcb, void* cbarg);
    int  Enable(short event);
    void SetWaterMark(short events, size_t lowmark, size_t highmark);
    SOCKET_T GetFd();
    int Connect(const struct sockaddr* addr, int socklen);

    void SetBufEv(bufferevent* pEv);
    bufferevent* BufEv();
private:
    struct bufferevent* _bufev = nullptr;
    NetEvBuffer         _EvBuffOut;
    NetEvBuffer         _EvBuffIn;
};

extern NetBufferEv* net_socket_new(NetEventBase* base, SOCKET_T fd, int options);
extern NetBufferEv* net_socket_new(NetEventBase* base, SOCKET_T fd, bool bCloseOnFree);
extern void net_bufferev_free(NetBufferEv* pBuffEv);
extern int net_socketpair(int d, int type, int protocol, SOCKET_T sv[2]);
extern bool net_is_connected_events(short events);