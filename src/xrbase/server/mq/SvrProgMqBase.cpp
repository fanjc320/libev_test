#include "pch.h"
#include <functional>
#include <typeinfo>

#include "SvrProgMqBase.h"
#include "platform/file_func.h"
#include "string/str_format.h"
#include "net/MsgParser.h"
#include "log/LogMgr.h"
#include "platform/func_stack.h"
#include "net/MsgParser.h"
#include "net/NetSetting.h"
#include "server/SvrMsgMake.h"
#include "server/SvrAppBase.h"
#include "time/time_func_x.h"

SvrProgMqBase::SvrProgMqBase(const std::string& szFrom, const std::string& szTo, int32_t max_buf)
{
    _strFrom = szFrom;
    _strTo = szTo;
    _max_buf = max_buf;
}

SvrProgMqBase::~SvrProgMqBase()
{

}

bool SvrProgMqBase::InitCreate(const std::string& szFrom, const std::string& szTo, int32_t max_buf)
{
    _strFrom = szFrom;
    _strTo = szTo;
    _max_buf = max_buf;

    bool ret = false;
    ret = _mqRecv.InitCreate(szTo, szFrom, max_buf, std::bind(&SvrProgMqBase::OnHandleMsgV, this, std::placeholders::_1, std::placeholders::_2));
    ERROR_LOG_EXIT0(ret);
    ret = _mqSend.InitCreate(szFrom, szTo, max_buf, std::bind(&SvrProgMqBase::OnHandleMsgV, this, std::placeholders::_1, std::placeholders::_2));
    ERROR_LOG_EXIT0(ret);
    return true;
Exit0:
    return false;
}

bool SvrProgMqBase::InitLoad(const std::string& szFrom, const std::string& szTo, int32_t max_buf)
{
    _strFrom = szFrom;
    _strTo = szTo;
    _max_buf = max_buf;

    bool ret = false;
    ret = _mqRecv.InitLoad(szTo, szFrom, std::bind(&SvrProgMqBase::OnHandleMsgV, this, std::placeholders::_1, std::placeholders::_2), max_buf);
    CHECK_LOG_ERROR(ret);
    ret = _mqSend.InitLoad(szFrom, szTo, std::bind(&SvrProgMqBase::OnHandleMsgV, this, std::placeholders::_1, std::placeholders::_2), max_buf);
    CHECK_LOG_ERROR(ret);

    return true;
Exit0:
    return false;
}

bool SvrProgMqBase::InitLoad()
{
    bool ret = false;
    ret = _mqRecv.InitLoad(_strTo, _strFrom, std::bind(&SvrProgMqBase::OnHandleMsgV, this, std::placeholders::_1, std::placeholders::_2), _max_buf);
    CHECK_LOG_ERROR(ret);
    ret = _mqSend.InitLoad(_strFrom, _strTo, std::bind(&SvrProgMqBase::OnHandleMsgV, this, std::placeholders::_1, std::placeholders::_2), _max_buf);
    CHECK_LOG_ERROR(ret);
    return true;
Exit0:
    return false;
}

void SvrProgMqBase::SetFromTo(const std::string& szFrom, const std::string& szTo, int32_t max_buf)
{
    _strFrom = szFrom;
    _strTo = szTo;
    _max_buf = max_buf;
}

void SvrProgMqBase::SetFrom(const std::string& szFrom)
{
    _strFrom = szFrom;
}

void SvrProgMqBase::SetTo(const std::string& szTo)
{
    _strTo = szTo;
}

bool SvrProgMqBase::OnHandleMsgV(const unsigned char* buf, uint32_t len)
{
    _tmRecvLast = get_curr_time();
  
    if (buf != nullptr && len >= 0)
    {
        if (_isConnect !=  EConState::Connect) _isConnect = EConState::FirstCon;

        MsgHead* pMsg = MSG_HEAD_CAST(buf);
        if (pMsg->flag == HEAD_FLAG_SVR_PING)
        {
            return SendPingRet();
        }
        else if (pMsg->flag == HEAD_FLAG_SVR_PING_RET)
        {
            return true;
        }

        return OnHandleMsg(MSG_HEAD_CAST(buf));
    }

    return false;
}

bool SvrProgMqBase::SendMsg(pb::Message* pMsg, msg_id_t usProto, uint8_t usModule /*= 0*/, uint16_t usCode /*= 0*/, uint32_t seqid /*= 0*/)
{
    std::string strStack;
    MsgHead* pNetMsg = nullptr;
    ERROR_LOG_EXIT0(pMsg != nullptr);
    pNetMsg = SvrMsgMake::MakeHeadToBuf(pMsg, usModule, usProto, seqid, usCode, _strBuf);

    ERROR_LOG_EXIT0(pNetMsg != nullptr);
    ERROR_LOG_EXIT0(_mqSend.Write((unsigned char*)pNetMsg, pNetMsg->size));
    _tmSendLast = get_curr_time();
    return true;

Exit0:
    func_stack::stack(strStack);
    LOG_ERROR("%s-%s", typeid(*this).name(), strStack.c_str());
    LOG_ERROR("MsgHead is null!, module:%u, cmd:%u, class:%s"
        , usModule, usProto, typeid(*this).name());
    return false;
}

bool SvrProgMqBase::SendMsg(const std::string& strMsg, msg_id_t usProto, uint8_t usModule /*= 0*/, uint16_t usCode /*= 0*/, uint32_t seqid /*= 0*/)
{
    return SendMsg(strMsg.c_str(), (uint32_t)strMsg.size(), usProto, usModule, usCode, seqid);
}

bool SvrProgMqBase::SendMsg(const char* buf, uint32_t size, msg_id_t usProto, uint8_t usModule /*= 0*/, uint16_t usCode /*= 0*/, uint32_t seqid /*= 0*/)
{
    std::string strStack;
    MsgHead* pNetMsg = nullptr;
    pNetMsg = SvrMsgMake::MakeHeadToBuf((char*)buf, size, usModule, usProto, seqid, usCode, _strBuf);
    ERROR_LOG_EXIT0(pNetMsg != nullptr);
    ERROR_LOG_EXIT0(_mqSend.Write((unsigned char*) pNetMsg, pNetMsg->size));
    _tmSendLast = get_curr_time();
    return true;

Exit0:
    func_stack::stack(strStack);
    LOG_ERROR("%s-%s", typeid(*this).name(), strStack.c_str());
    LOG_ERROR("MsgHead is null!, module:%u, cmd:%u, class:%s"
        , usModule, usProto, typeid(*this).name());
    return false;
}

bool SvrProgMqBase::SendMsg(const MsgHead* pMsg)
{
    std::string strStack;
    ERROR_LOG_EXIT0(_mqSend.Write((unsigned char*) pMsg, pMsg->size));
    _tmSendLast = get_curr_time();
    return true;
Exit0:
    func_stack::stack(strStack);
    LOG_ERROR("%s-%s", typeid(*this).name(), strStack.c_str());
    LOG_ERROR("MsgHead is null!, module:%u, cmd:%u, class:%s"
        , pMsg->mod_id, pMsg->proto_id, typeid(*this).name());
    return false;
}

bool SvrProgMqBase::SendPing()
{
    MsgHead head;
    head.flag = HEAD_FLAG_SVR_PING;
    head.mod_id = UINT8_MAX;
    head.proto_id = UINT8_MAX;
    head.code = 0;
    head.seq_id = 0;
    head.size = MSG_HEAD_SIZE;
    return SendMsg(&head);
}

bool SvrProgMqBase::SendPingRet()
{
    MsgHead head;
    head.flag = HEAD_FLAG_SVR_PING_RET;
    head.mod_id = UINT8_MAX;
    head.proto_id = UINT8_MAX;
    head.code = 0;
    head.seq_id = 0;
    head.size = MSG_HEAD_SIZE;
    return SendMsg(&head);
}

void SvrProgMqBase::MainLoop(int32_t times /*= 100*/)
{
    _mqRecv.MainLoop(times);
}