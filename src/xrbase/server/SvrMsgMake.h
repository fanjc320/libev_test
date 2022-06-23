#pragma once
#include "net/NetSetting.h"
#include <stdint.h>
#include "dtype.h"

namespace google
{
    namespace protobuf
    {
        class Message;
    };
};

struct NetMsg;
struct MsgHead;
namespace SvrMsgMake
{    
    extern MsgHead* MakeHeadToBuf(const pb::Message* pMessage, uint8_t u8Module, uint8_t u8Proto, uint32_t uiSeqID,
        uint16_t usCode, std::string& out_string);

    extern MsgHead* MakeHeadToBuf(const std::string& strMsg, uint8_t u8Module, uint8_t u8Proto,
        uint32_t uiSeqID, uint16_t usCode, std::string& out_string);

    extern MsgHead* MakeHeadToBuf(const char* buf, uint32_t size, uint8_t u8Module, uint8_t u8Proto,
        uint32_t uiSeqID, uint16_t usCode, std::string& out_string);

    extern NetMsg* MakeNetMsg(const char* buf, uint32_t size, uint8_t u8Module, uint8_t u8Proto,
        uint32_t uiSeqID, uint16_t usCode, uint16_t conn_count);
    extern NetMsg* MakeNetMsg(const pb::Message* pMessage, uint8_t u8Module, uint8_t u8Proto, uint32_t uiSeqID,
        uint16_t usCode, uint16_t conn_count);
    extern NetMsg* MakeNetMsg(const std::string& strMsg, uint8_t u8Module, uint8_t u8Proto,
        uint32_t uiSeqID, uint16_t usCode, uint16_t conn_count);
    extern NetMsg* MakeNetMsg(const MsgHead* pMsgHead, conn_oid_t coid_count);
    extern NetMsg* MakeNetMsgCopy(const NetMsg* pNetMsg);

};

