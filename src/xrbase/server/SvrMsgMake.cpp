#include "pch.h"
#include "dtype.h"
#include <typeinfo>

#include "server/SvrMsgMake.h"
#include "google/protobuf/message.h"
#include "net/NetInclude.h"
#include "platform/func_stack.h"
#include "pb/pb_parse.h"
#include "log/LogMgr.h"
#include "memory/malloc_x.h"

namespace SvrMsgMake
{
    MsgHead* MakeHeadToBuf(const pb::Message* pMessage, uint8_t u8Module, uint8_t u8Proto, uint32_t uiSeqID, uint16_t usCode, std::string& out_string)
    {
        MsgHead* pMsgHead = nullptr;
        if (pMessage == nullptr) return nullptr;
        uint32_t nSize = (uint32_t) pMessage->ByteSizeLong();
        unsigned char* pDataHead = nullptr;

        out_string.resize((size_t) nSize + MSG_HEAD_SIZE);
        pMsgHead = MSG_HEAD_CAST(out_string.data());
        pDataHead = pMsgHead->body;
        ERROR_LOG_EXIT0(pb2_array(pMessage, pDataHead, nSize));

        pMsgHead = make_head_msg_to_buf((char*) pDataHead, nSize, out_string.data(), u8Module, u8Proto, uiSeqID, usCode);
        ERROR_LOG_EXIT0(pMsgHead != nullptr);
        return pMsgHead;

Exit0:
        {
            std::string strStack;
            func_stack::stack(strStack);
            LOG_ERROR("%s, sendlen %s : %d, module: %u, protol: %u", strStack.c_str(),
                typeid(*pMessage).name(), nSize, u8Module, u8Proto);
        }
        return nullptr;
    }

    MsgHead* MakeHeadToBuf(const char* buf, uint32_t size, uint8_t u8Module, uint8_t u8Proto, uint32_t uiSeqID, uint16_t usCode, std::string& out_string)
    {
        MsgHead* pMsgHead = nullptr;
        if (buf == nullptr) return nullptr;
        unsigned char* pDataHead = nullptr;

        out_string.resize((size_t) size + MSG_HEAD_SIZE);
        pMsgHead = make_head_msg_to_buf(buf, size, out_string.data(), u8Module, u8Proto, uiSeqID, usCode);
        ERROR_LOG_EXIT0(pMsgHead != nullptr);
        return pMsgHead;

Exit0:
        {
            std::string strStack;
            func_stack::stack(strStack);
            LOG_ERROR("%s, sendlen %d, module: %u, protol: %u", strStack.c_str(), size, u8Module, u8Proto);
        }
        return nullptr;
    }

    MsgHead* MakeHeadToBuf(const std::string& strMsg, uint8_t u8Module, uint8_t u8Proto, uint32_t uiSeqID, uint16_t usCode, std::string& out_string)
    {
        return MakeHeadToBuf(strMsg.c_str(), (uint32_t) strMsg.length(), u8Module, u8Proto, uiSeqID, usCode, out_string);
    }

    NetMsg* MakeNetMsg(const pb::Message* pMessage
        , uint8_t u8Module, uint8_t u8Proto, uint32_t uiSeqID
        , uint16_t usCode, uint16_t conn_count)
    {
        NetMsg* pNetMsg = nullptr;
        MsgHead* pMsgHead = nullptr;
        if (pMessage == nullptr) return nullptr;
        uint32_t nSize = (uint32_t) pMessage->ByteSizeLong();
        unsigned char* pDataHead = nullptr;

        pNetMsg = make_net_msg(nSize, u8Module, u8Proto, uiSeqID, usCode, conn_count);
        ERROR_LOG_EXIT0(pNetMsg != nullptr);
        pMsgHead = MSG_HEAD_FROM_NET(pNetMsg);
        ERROR_LOG_EXIT0(pMsgHead != nullptr);

        pDataHead = MSG_HEAD_BUF(pMsgHead);
        ERROR_LOG_EXIT0(pb2_array(pMessage, pDataHead, nSize));
        return pNetMsg;
Exit0:
        if (pNetMsg) MSG_FREE(pNetMsg);
        {
            std::string strStack;

            func_stack::stack(strStack);
            LOG_ERROR("%s, sendlen %s : %d, module: %u, protol: %u", strStack.c_str(),
                typeid(*pMessage).name(), nSize, u8Module, u8Proto);
        }
        return nullptr;
    }

    NetMsg* MakeNetMsg(const std::string& strMsg
        , uint8_t u8Module, uint8_t u8Proto, uint32_t uiSeqID
        , uint16_t usCode, uint16_t conn_count)
    {
        return MakeNetMsg(strMsg.c_str(), (uint32_t)strMsg.length(), u8Module, u8Proto, uiSeqID, usCode, conn_count);
    }

    NetMsg* MakeNetMsg(const char* buf, uint32_t size
        , uint8_t u8Module, uint8_t u8Proto, uint32_t uiSeqID
        , uint16_t usCode, uint16_t conn_count)
    {
        NetMsg* pNetMsg = nullptr;

        pNetMsg = make_net_msg(buf, size, u8Module, u8Proto, uiSeqID, usCode, conn_count);
        ERROR_LOG_EXIT0(pNetMsg != nullptr);
        return pNetMsg;

Exit0:
        if (pNetMsg) MSG_FREE(pNetMsg);
        {
            std::string strStack;
            func_stack::stack(strStack);
            LOG_ERROR("%s, sendlen : %d, module: %u, protol: %u", strStack.c_str(), size, u8Module, u8Proto);

        }
        return nullptr;
    }

    NetMsg* MakeNetMsg(const MsgHead* pMsgHead, conn_oid_t coid_count)
    {
        NetMsg* pNetMsg = make_mutil_net_msg(pMsgHead, coid_count);
        if (pNetMsg == nullptr)
        {
            std::string strStack;
            func_stack::stack(strStack);
            LOG_ERROR("%s, coid_count : %u", strStack.c_str(), coid_count);
        }
        return pNetMsg;
    }

    NetMsg* MakeNetMsgCopy(const NetMsg* pNetMsg)
    {
        NetMsg* pNewMsg = net_msg_alloc(pNetMsg->size);
        if (pNewMsg == nullptr)
        {
            std::string strStack;
            func_stack::stack(strStack);
            LOG_ERROR("%s, coid_count : %u", strStack.c_str(), pNetMsg->size);
            return nullptr;
        }
        memcpy(pNewMsg, pNetMsg, pNetMsg->size);
        return pNewMsg;
    }
}

