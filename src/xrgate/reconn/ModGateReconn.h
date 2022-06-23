#pragma once
#include "dtype.h"
#include "server/mq/SvrProgMqHandle.h"
#include "server/SvrModuleBase.h"
#include "util/SingleTon.h"
#include "server/SvrKindList.h"

class ReconnCenter;
class ReconnFriend;
class ReconnHttp;
class ReconnLog;
class ReconnMail;
class ReconnGame;
class GatePlayer;

class ModGateReconn : public SvrModuleBase, public SingleTon<ModGateReconn>
{
public:
	virtual bool LogicInit() override;
	virtual void LogicFini() override;

	virtual bool Reload() override;
	virtual void OnFrame(uint64_t qwTm) override;
	virtual void OnTick(uint64_t qwTm) override; //一毫秒一次

	virtual bool TransClientToSvr(E_SVR_KIND_TYPE eType, GatePlayer* pPlayer, const MsgHead* pMsg);
	virtual bool SendMsgToSvr(E_SVR_KIND_TYPE eType, pb::Message* pMsg, msg_id_t usProto
		, uint8_t usModule = 0, uint16_t usCode = 0, uint32_t seqid = 0);

	//如果下面的链接是数组，需要改成数组形式，或者用SvrKindList代替
	ReconnCenter* GetCenter();
	ReconnFriend* GetFriend();
	ReconnHttp* GetHttp();
	ReconnLog* GetLog();
	ReconnMail* GetMail();
	ReconnGame* GetGame();

protected:
	SvrProgMqHandle _Handle;
	//如果下面的链接是数组，需要改成数组形式，或者用SvrKindList代替

	ReconnCenter* _pCenter = nullptr;
	ReconnFriend* _pFriend = nullptr;
	ReconnHttp* _pHttp = nullptr;
	ReconnLog* _pLog = nullptr;
	ReconnMail* _pMail = nullptr;
	ReconnGame* _pGame = nullptr;
};
