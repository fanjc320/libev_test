#include "pch.h"
#include "ModGateReconn.h"
#include "reconn/ReconnCenter.h"
#include "reconn/ReconnFriend.h"
#include "reconn/ReconnHttp.h"
#include "reconn/ReconnLog.h"
#include "reconn/ReconnMail.h"
#include "reconn/ReconnGame.h"
#include "server/SvrAppBase.h"
#include "log/LogMgr.h"
#include "server/MsgSvrCommon.pb.h"
#include "session/GatePlayer.h"
#include "pb/pb_parse.h"
#include "server/MsgSvrModule.pb.h"
#include "PbConst.h"

bool ModGateReconn::LogicInit()
{
    bool ret = false;

	int32_t id = 1;//负载时，需要针对单个链接设置id
	int32_t arean = GetMainApp()->GetAreanNo();

	AddConnT(arean, id, _pCenter, ReconnCenter);
	AddConnT(arean, id, _pFriend, ReconnFriend);
	AddConnT(arean, id, _pHttp, ReconnHttp);
	AddConnT(arean, id, _pLog, ReconnLog);
	AddConnT(arean, id, _pMail, ReconnMail);
	AddConnT(arean, id, _pGame, ReconnGame);

    return SvrModuleBase::LogicInit();

Exit0:
    return false;
}

void ModGateReconn::LogicFini()
{
    _Handle.FreeReconT();
	_pCenter = nullptr;
	_pFriend = nullptr;
	_pHttp = nullptr;
	_pLog = nullptr;
	_pMail = nullptr;
	_pGame = nullptr;
}

bool ModGateReconn::Reload()
{
    return SvrModuleBase::Reload();
}

void ModGateReconn::OnFrame(uint64_t qwTm)
{
    for (auto& itr : _Handle.GetMap())
    {
        itr->OnFrame(qwTm);
    }
}

void ModGateReconn::OnTick(uint64_t qwTm)
{
    for (auto& itr : _Handle.GetMap())
    {
        itr->OnTick(qwTm);
    }
}

bool ModGateReconn::TransClientToSvr(E_SVR_KIND_TYPE eType, GatePlayer* pPlayer, const MsgHead* pMsg)
{
	Msg_SS_TransData msg;
	msg.add_acc_lst(pPlayer->GetAccId());
	msg.set_mod_id(pMsg->mod_id);
	msg.set_proto_id(pMsg->proto_id);
	msg.set_ret_code(0);
	msg.set_seq_id(pMsg->seq_id);
	msg.mutable_proto_msg()->append((char*)pMsg, pMsg->size);
	
	return SendMsgToSvr(eType, &msg, SvrMod_Com::SvrInner, MsgSvrMod::NetSvrCommon, pMsg->code, pMsg->seq_id);
}

bool ModGateReconn::SendMsgToSvr(E_SVR_KIND_TYPE eType, pb::Message* pMsg, msg_id_t usProto
	, uint8_t usModule, uint16_t usCode, uint32_t seqid)
{
	switch (eType)
	{
	case SVR_KIND_CENTER:
		return _pCenter->SendMsg(pMsg, usProto, usModule, usCode, seqid);
		break;
	case SVR_KIND_FRIEND:
		return _pFriend->SendMsg(pMsg, usProto, usModule, usCode, seqid);
		break;
	case SVR_KIND_GAME:
		return _pGame->SendMsg(pMsg, usProto, usModule, usCode, seqid);
		break;
	case SVR_KIND_MAIL:
		return _pMail->SendMsg(pMsg, usProto, usModule, usCode, seqid);
		break;
	case SVR_KIND_HTTP:
		return _pHttp->SendMsg(pMsg, usProto, usModule, usCode, seqid);
		break;
	case SVR_KIND_LOG:
		return _pLog->SendMsg(pMsg, usProto, usModule, usCode, seqid);
		break;
	default:
		LOG_WARNING("not set svr_type send mod: %d, proto:%d, tosvr: %d", usModule, usProto, eType);
		break;
	}
	return true;
}

ReconnCenter* ModGateReconn::GetCenter()
{
	return _pCenter;
}

ReconnFriend* ModGateReconn::GetFriend()
{
	return _pFriend;
}

ReconnHttp* ModGateReconn::GetHttp()
{
	return _pHttp;
}

ReconnLog* ModGateReconn::GetLog()
{
	return _pLog;
}

ReconnMail* ModGateReconn::GetMail()
{
	return _pMail;
}

ReconnGame* ModGateReconn::GetGame()
{
	return _pGame;
}
