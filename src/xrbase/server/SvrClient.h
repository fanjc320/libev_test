/*
 *
 *      Author: venture
 */
#pragma once

#include <string>
#include <list>
#include <stdint.h>
#include "dtype.h"
#include "net/LogicHandle.h"
#include "net/NetClient.h"
#include "server/SvrKindID.h"
#include "server/SvrKindHandle.h"
#include "pb/pb_parse.h"

namespace google
{
    namespace protobuf
    {
        class Message;
    };
};

class SvrLogicHandle;
class SvrReconn : public NetReconn, public SvrKindID
{
public:
    SvrReconn(bool bCache = true, uint32_t cacheCount = MAX_CACHE_COUNT);
    ~SvrReconn();

	virtual bool SendMsg(pb::Message* pMsg, msg_id_t usProto
		, uint8_t usModule = 0, uint16_t usCode = 0, uint32_t seqid = 0);

    virtual bool SendMsg(const std::string& strMsg, msg_id_t usProto
        , uint8_t usModule = 0, uint16_t usCode = 0, uint32_t seqid = 0);

    virtual bool SendMsg(const char* buf, uint32_t size, msg_id_t usProto
        , uint8_t usModule = 0, uint16_t usCode = 0, uint32_t seqid = 0);

    virtual bool Send(const MsgHead* pMsg);
    virtual bool Send(const NetMsg* pMsg);

    virtual bool OnHandleMsgV(const NetMsg* pMsg) override;
    virtual int  OnEvent(uint32_t id, const MsgHead* pMsg) = 0;
    virtual bool OnEventDefault(const MsgHead* pMsg) { return 0; }
};

class SvrSession : public NetSession, public SvrKindID
{
public:
    SvrSession();
    ~SvrSession();

	virtual bool SendMsg(pb::Message* pMsg, msg_id_t usProto
        , uint8_t usModule = 0, uint16_t usCode = 0, uint32_t seqid = 0);
    virtual bool SendMsg(const std::string& strMsg, msg_id_t usProto
        , uint8_t usModule = 0, uint16_t usCode = 0, uint32_t seqid = 0);
    virtual bool SendMsg(const char* buf, uint32_t size, msg_id_t usProto
        , uint8_t usModule = 0, uint16_t usCode = 0, uint32_t seqid = 0);
    virtual bool Send(const MsgHead* pMsg);
    virtual bool Send(const NetMsg* pMsg);

    virtual bool OnHandleMsgV(const NetMsg* pMsg) override;
    virtual int  OnEvent(uint32_t id, const MsgHead* pMsg) = 0;
    virtual bool OnEventDefault(const MsgHead* pMsg) { return 0; }

};

