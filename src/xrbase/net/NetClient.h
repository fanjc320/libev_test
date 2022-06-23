/*
 *
 *      Author: venture
 */
#pragma once
#include <string>
#include <list>
#include "net/LogicHandle.h"

class LogicThread;
struct MsgHead;

class NetSession : public LogicSession
{
protected:
    virtual bool _Send(NetMsg* pMsg);
    virtual bool _Send(const MsgHead* pMsg);
};

class NetReconn : public ReconnSession
{
public:
    static const uint32_t MAX_CACHE_COUNT = 10000;
    NetReconn(bool bCache = false, uint32_t cacheCount = MAX_CACHE_COUNT);
    ~NetReconn();

    virtual bool AddCacheMsg(const char* pMsg, uint32_t len, bool is_head);
    virtual void OnConnect();

protected:
    virtual bool _Send(NetMsg* pMsg);
    virtual bool _Send(const MsgHead* pMsg);

private:
    bool        _bCacheSend = false;
    uint32_t    _cacheCount = MAX_CACHE_COUNT;
    struct SCACHE
    {
        bool is_head;
        std::string buf;
    };
    std::list<SCACHE> _lstCache;
};
