/*
 *
 *      Author: venture
 */
#pragma once

#include "dtype.h"
#include "net/NetInclude.h"
namespace google
{
    namespace protobuf
    {
        class Message;
		class MessageLite;
    };
};

extern void set_pb_log_handle();
extern bool pb2_array(const pb::MessageLite* pMsg, void* pStr, size_t len);
extern bool pb2_array(const pb::MessageLite& oMsg, std::string& oStr);
extern bool pb2_array(const pb::MessageLite* pMsg, std::string* pStr);
extern bool array2_pb(pb::MessageLite* pMsg, const void* pData, size_t nSize);

#define PARSE_PTL(oProto, pData, nSize) \
	if (!array2_pb(&(oProto), (pData), (nSize))) \
		return; \

#define PARSE_PTL_STR(oProto, strBuf) \
		PARSE_PTL((oProto), (strBuf).c_str(), (strBuf).size())

#define PARSE_PTL_HEAD(oProto, head) \
		PARSE_PTL((oProto), (MSG_HEAD_BUF(head)), (MSG_HEAD_SIZE(head)))

#define PARSE_PTL_RET(oProto, pData, nSize) \
	if (!array2_pb(&(oProto), (pData), (nSize))) \
		return false; \

#define PARSE_PTL_STR_RET(oProto, strBuf) \
		PARSE_PTL_RET((oProto), (strBuf).c_str(), (strBuf).size())

#define PARSE_PTL_HEAD_RET(oProto, head) \
		PARSE_PTL_RET((oProto), (MSG_HEAD_BUF(head)), (MSG_HEAD_SIZE(head)))

