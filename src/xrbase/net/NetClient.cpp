/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <typeinfo>

#include "NetClient.h"
#include "MsgParser.h"
#include "net/LogicThread.h"
#include "log/LogMgr.h"
#include "time/time_func_x.h"
#include "base/IoThreadControl.h"
#include "memory/malloc_x.h"

bool NetSession::_Send(NetMsg* pMsg)
{
    if (_pIoThreadCtl != nullptr && IsConnected())
        return LogicSession::SendNet(pMsg);
    else
        MSG_FREE(pMsg);
    return false;
}

bool NetSession::_Send(const MsgHead* pMsg)
{
    if (_pIoThreadCtl != nullptr && IsConnected())
        return LogicSession::SendNet(pMsg);
    return false;
}

NetReconn::NetReconn(bool bCache, uint32_t cacheCount)
    : _bCacheSend(bCache), _cacheCount(cacheCount)
{ }

NetReconn::~NetReconn()
{
    _lstCache.clear();
}

bool NetReconn::AddCacheMsg(const char* pMsg, uint32_t len, bool is_head)
{
    if (_bCacheSend)
    {
        //只缓存突然断开的，其他时间不保存
        if (_lstCache.size() < _cacheCount)
        {
            _lstCache.push_back(SCACHE(is_head, std::string(pMsg, len)));
            return true;
        }
        else
        {
            LOG_ERROR("max size:%d, name:%s", _lstCache.size(), typeid(*this).name());
        }
    }
    return false;
}

bool NetReconn::_Send(NetMsg* pMsg)
{
    if (IsConnected() && _pIoThreadCtl != nullptr)
    {
        return LogicSession::SendNet(pMsg);
    }
    else
    {
        AddCacheMsg((const char*) pMsg, pMsg->size, false);
        MSG_FREE(pMsg);
    }
    return false;
}

bool NetReconn::_Send(const MsgHead* pMsg)
{
    if (IsConnected() && _pIoThreadCtl != nullptr)
    {
        return LogicSession::SendNet(pMsg);
    }
    else
    {
        AddCacheMsg((const char*) pMsg, pMsg->size, true);
    }
    return false;
}

void NetReconn::OnConnect()
{
    if (_bCacheSend && _pIoThreadCtl != nullptr)
    {
        for (auto itr = _lstCache.begin(); itr != _lstCache.end(); ++itr)
        {
            if(itr->is_head)
                LogicSession::SendNet((MsgHead*) (itr->buf.c_str()));
            else
                LogicSession::SendNet((NetMsg*) (itr->buf.c_str()));
        }
        _lstCache.clear();
    }
}