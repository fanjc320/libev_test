/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <typeinfo>

#include "dtype.h"
#include "SvrClient.h"
#include "log/LogMgr.h"
#include "platform/func_stack.h"
#include "net/MsgParser.h"
#include "net/NetSetting.h"
#include "server/SvrMsgMake.h"
#include "math/math_basic.h"

SvrReconn::SvrReconn(bool bCache, uint32_t cacheCount)
    : NetReconn(bCache, cacheCount)
{ }

SvrReconn::~SvrReconn()
{ }

bool SvrReconn::SendMsg(pb::Message* pMsg
    , msg_id_t usProto, uint8_t usModule, uint16_t usCode, uint32_t seqid)
{
    std::string strStack;
    NetMsg* pNetMsg = nullptr;
    ERROR_LOG_EXIT0(pMsg != nullptr);
    pNetMsg = SvrMsgMake::MakeNetMsg(pMsg, usModule, usProto, seqid, usCode, 1);
    ERROR_LOG_EXIT0(pNetMsg != nullptr);
    ERROR_LOG_EXIT0(NetReconn::_Send(pNetMsg));
    return true;

Exit0:
    func_stack::stack(strStack);
    LOG_ERROR("%s-%s", typeid(*this).name(), strStack.c_str());
    LOG_ERROR("MsgHead is null!, module:%u, cmd:%u, class:%s"
        , usModule, usProto, typeid(*this).name());
    return false;
}

bool SvrReconn::SendMsg(const std::string& strMsg, msg_id_t usProto
    , uint8_t usModule, uint16_t usCode, uint32_t seqid)
{
    return SendMsg(strMsg.c_str(), (uint32_t)strMsg.length(), usProto, usModule, usCode, seqid);
}

bool SvrReconn::SendMsg(const char* buf, uint32_t size
    , msg_id_t usProto, uint8_t usModule, uint16_t usCode, uint32_t seqid)
{
    NetMsg* pNetMsg = SvrMsgMake::MakeNetMsg((const char*) buf, size, usModule, usProto, seqid, usCode, 1);
    std::string strStack;
    ERROR_LOG_EXIT0(pNetMsg != nullptr);
    ERROR_LOG_EXIT0(NetReconn::_Send(pNetMsg));
    return true;
Exit0:
    func_stack::stack(strStack);
    LOG_ERROR("%s-%s", typeid(*this).name(), strStack.c_str());

    LOG_ERROR("MsgHead is null!, module:%u, cmd:%u, class:%s"
        , usModule, usProto, typeid(*this).name());
    return false;
}

bool SvrReconn::Send(const MsgHead* pHeadMsg)
{
    std::string strStack;
    bool bRes = NetReconn::_Send(pHeadMsg);
    ERROR_LOG_EXIT0(bRes);
    return true;
Exit0:
    func_stack::stack(strStack);
    LOG_ERROR("%s", strStack.c_str());
    return false;
}

bool SvrReconn::Send(const NetMsg* pMsg)
{
    std::string strStack;
    NetMsg* pNetMsg = SvrMsgMake::MakeNetMsgCopy(pMsg);
    bool bRes = NetReconn::_Send(pNetMsg);
    ERROR_LOG_EXIT0(bRes);
    return true;
Exit0:
    func_stack::stack(strStack);
    LOG_ERROR("%s", strStack.c_str());
    return false;
}

bool SvrReconn::OnHandleMsgV(const NetMsg* pMsg)
{
    const MsgHead* pHead = MSG_HEAD_FROM_NET(pMsg);
    if (pHead != nullptr)
    {
		int ret = OnEvent(MAKE_UINT32(pHead->mod_id, pHead->proto_id), pHead);
		if (ret >= 0) return ret;
		return OnEventDefault(pHead);
    }

    return false;
}

SvrSession::SvrSession()
{ }


SvrSession::~SvrSession()
{ }

bool SvrSession::SendMsg(pb::Message* pMsg, msg_id_t usProto
    , uint8_t usModule, uint16_t usCode, uint32_t seqid)
{
    NetMsg* pNetMsg = nullptr;
    ERROR_LOG_EXIT0(pMsg != nullptr);
    pNetMsg = SvrMsgMake::MakeNetMsg(pMsg, usModule
        , usProto, seqid, usCode, 1);
    ERROR_LOG_EXIT0(pNetMsg != nullptr);
    ERROR_LOG_EXIT0(NetSession::_Send(pNetMsg));
    return true;

Exit0:
    std::string strStack;
    func_stack::stack(strStack);
    LOG_ERROR("%s-%s", typeid(*this).name(), strStack.c_str());

    LOG_ERROR("MsgHead is null!, module:%u, cmd:%u, class:%s"
        , usModule, usProto, typeid(*this).name());
    return false;
}

bool SvrSession::SendMsg(const std::string& strMsg, msg_id_t usProto
    , uint8_t usModule, uint16_t usCode, uint32_t seqid)
{
    return SendMsg(strMsg.c_str(), (uint32_t)strMsg.length(), usProto, usModule, usCode, seqid);
}

bool SvrSession::SendMsg(const  char* buf, uint32_t size
    , msg_id_t usProto, uint8_t usModule, uint16_t usCode, uint32_t seqid)
{
    std::string strStack;

    NetMsg* pNetMsg = SvrMsgMake::MakeNetMsg((const char*) buf, size, usModule
        , usProto, seqid, usCode, 1);
    ERROR_LOG_EXIT0(pNetMsg != nullptr);
    ERROR_LOG_EXIT0(NetSession::_Send(pNetMsg));
    return true;
Exit0:
    func_stack::stack(strStack);
    LOG_ERROR("%s-%s", typeid(*this).name(), strStack.c_str());

    LOG_ERROR("MsgHead is null!, module:%u, cmd:%u, class:%s"
        , usModule, usProto, typeid(*this).name());
    return false;
}

bool SvrSession::OnHandleMsgV(const NetMsg* pMsg)
{
	const MsgHead* pHead = MSG_HEAD_FROM_NET(pMsg);
    if (pHead != nullptr)
    {
        int ret = OnEvent(MAKE_UINT32(pHead->mod_id, pHead->proto_id), pHead);
        if (ret >= 0) return ret;
        return OnEventDefault(pHead);
    }

    return false;
}

bool SvrSession::Send(const MsgHead* pHeadMsg)
{
    std::string strStack;
    bool bRes = NetSession::_Send(pHeadMsg);
    ERROR_LOG_EXIT0(bRes);
    return true;
Exit0:
    func_stack::stack(strStack);
    LOG_ERROR("%s", strStack.c_str());
    return false;
}

bool SvrSession::Send(const NetMsg* pMsg)
{
    std::string strStack;
    NetMsg* pNetMsg = SvrMsgMake::MakeNetMsgCopy(pMsg);
    bool bRes = NetSession::_Send(pNetMsg);
    ERROR_LOG_EXIT0(bRes);
    return true;
Exit0:
    func_stack::stack(strStack);
    LOG_ERROR("%s", strStack.c_str());
    return false;
}
