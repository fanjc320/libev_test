#pragma once
#include "dtype.h"
#include "mq/shared/ProgMqBase.h"
#include "shm/shm_x.h"
#include "text/ring_buf.h"
#include "net/NetSetting.h"
#include "server/SvrKindID.h"
#include "net/MsgParser.h"
#include "time/TimeCount.h"
#include "time/time_func_x.h"

namespace google
{
    namespace protobuf
    {
        class Message;
    };
};


struct MsgHead;
class SvrProgMqBase : public SvrKindID
{
public:
	enum class EConState
	{
		None,
		Connect,
		Discon,
		FirstCon
	};

    SvrProgMqBase() = default;
    virtual ~SvrProgMqBase();

    SvrProgMqBase(const std::string& szFrom, const std::string& szTo, int32_t max_buf);

    bool InitCreate(const std::string& szFrom, const std::string& szTo, int32_t max_buf);
    bool InitLoad(const std::string& szFrom, const std::string& szTo, int32_t max_buf = 0);
    bool InitLoad();
    void SetFromTo(const std::string& szFrom, const std::string& szTo, int32_t max_buf = 0);
	void SetFrom(const std::string& szFrom);
	void SetTo(const std::string& szTo);

    //default recv mq pop
    virtual bool OnHandleMsgV(const unsigned char* buf, uint32_t len);
    //default recv mq pop
    virtual bool OnHandleMsg(const MsgHead* pMsgHead) = 0;

    virtual bool SendMsg(pb::Message* pMsg, msg_id_t usProto
        , uint8_t usModule = 0, uint16_t usCode = 0, uint32_t seqid = 0);

    virtual bool SendMsg(const std::string& strMsg, msg_id_t usProto
        , uint8_t usModule = 0, uint16_t usCode = 0, uint32_t seqid = 0);

    virtual bool SendMsg(const char* buf, uint32_t size, msg_id_t usProto
        , uint8_t usModule = 0, uint16_t usCode = 0, uint32_t seqid = 0);

    virtual bool SendMsg(const MsgHead* pMsg);

    bool SendPing();
    bool SendPingRet();

    virtual void MainLoop(int32_t times = 100);

    bool IsInit() const { return _mqRecv.IsInit() && _mqSend.IsInit(); }

    const std::string& GetSendKey() const { return _mqSend.GetKey(); }
    const std::string& GetFromName() const { return _mqSend.GetFromName(); }
    const std::string& GetToName() const { return _mqSend.GetToName(); }

protected:
    std::string     _strBuf;
    ProgMqBase      _mqSend;
    ProgMqBase      _mqRecv;
    std::string     _strFrom;
    std::string     _strTo;
    int32_t         _max_buf = 0;
    EConState       _isConnect = EConState::None;

    time_t          _tmRecvLast = get_curr_time();
    time_t          _tmSendLast = 0;
};