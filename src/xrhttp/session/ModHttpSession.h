#pragma once
#include "dtype.h"
#include "server/SvrModuleBase.h"
#include "util/SingleTon.h"
#include "server/mq/SvrProgMq.h"
#include "server/mq/SvrProgMqHandle.h"
#include "server/SvrKindList.h"

class GameSession;
class GateSession;

class ModHttpSession :
    public SvrModuleBase, public SingleTon<ModHttpSession>
{
public:
	virtual bool LogicInit() override;
	virtual void LogicFini() override;

	virtual bool Reload() override;
	virtual void OnFrame(uint64_t qwTm) override;
	virtual void OnTick(uint64_t qwTm) override; //“ª∫¡√Î“ª¥Œ

	virtual bool SendMsgToSvr(E_SVR_KIND_TYPE eType, uint32_t svr_id, pb::Message* pMsg, msg_id_t usProto
		, uint8_t usModule = 0, uint16_t usCode = 0, uint32_t seqid = 0);
	virtual bool SendMsgToGate(uint32_t svr_id, pb::Message* pMsg, msg_id_t usProto
		, uint8_t usModule = 0, uint16_t usCode = 0, uint32_t seqid = 0);
	virtual bool SendMsgToGame(uint32_t svr_id, pb::Message* pMsg, msg_id_t usProto
		, uint8_t usModule = 0, uint16_t usCode = 0, uint32_t seqid = 0);

	GameSession* GetGame(uint32_t id);
	GateSession* GetGate(uint32_t id);

protected:
	SvrProgMqHandle _Handle;
	SvrKindList		_lstSes;
};

