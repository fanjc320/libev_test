#include "pch.h"
#include "ModHttpSession.h"
#include "GameSession.h"
#include "GateSession.h"
#include "server/SvrAppBase.h"

bool ModHttpSession::LogicInit()
{
	//负载时，需要从redis读取出gate, game数量
	int32_t nGateNum = 1;
	int32_t nGameNum = 1;
	int32_t arean = GetMainApp()->GetAreanNo();

	for (int id = 1; id <= nGateNum; ++id)
	{
		GateSession* pNew = nullptr;
		AddConnT(arean, id, pNew, GateSession);
		_lstSes.AddID(pNew->GetSvrType(), pNew->GetSvrID(), pNew);
	}
	for (int id = 1; id <= nGameNum; ++id)
	{
		GameSession* pNew = nullptr;
		AddConnT(arean, id, pNew, GameSession);
		_lstSes.AddID(pNew->GetSvrType(), pNew->GetSvrID(), pNew);
	}

	return SvrModuleBase::LogicInit();
Exit0:
	return false;
}

void ModHttpSession::LogicFini()
{
	_Handle.FreeReconT();
	_lstSes.Clear();
}

bool ModHttpSession::Reload()
{
	return SvrModuleBase::Reload();
}

void ModHttpSession::OnFrame(uint64_t qwTm)
{
	for (auto& itr : _Handle.GetMap())
	{
		itr->OnFrame(qwTm);
	}
}

void ModHttpSession::OnTick(uint64_t qwTm)
{
	for (auto& itr : _Handle.GetMap())
	{
		itr->OnTick(qwTm);
	}
}

bool ModHttpSession::SendMsgToSvr(E_SVR_KIND_TYPE eType, uint32_t svr_id, pb::Message* pMsg, 
	msg_id_t usProto, uint8_t usModule /*= 0*/, uint16_t usCode /*= 0*/, uint32_t seqid /*= 0*/)
{
	switch (eType)
	{
	case SVR_KIND_GAME:
	{
		GameSession* pGame = GetGame(svr_id);
		if (pGame != nullptr)
		{
			return pGame->SendMsg(pMsg, usProto, usModule, usCode, seqid);
		}
	}
	break;
	case SVR_KIND_GATE:
	{
		GateSession* pGate = GetGate(svr_id);
		if (pGate != nullptr)
		{
			return pGate->SendMsg(pMsg, usProto, usModule, usCode, seqid);
		}
	}
	break;
	default:
		LOG_WARNING("not set svr_type send mod: %d, proto:%d, tosvr: %d", usModule, usProto, eType);
		break;
	}

	return true;
}

bool ModHttpSession::SendMsgToGate(uint32_t svr_id, pb::Message* pMsg, msg_id_t usProto,
	uint8_t usModule /*= 0*/, uint16_t usCode /*= 0*/, uint32_t seqid /*= 0*/)
{
	return SendMsgToSvr(SVR_KIND_GATE, svr_id, pMsg, usProto, usModule, usCode, seqid);
}

bool ModHttpSession::SendMsgToGame(uint32_t svr_id, pb::Message* pMsg, msg_id_t usProto,
	uint8_t usModule /*= 0*/, uint16_t usCode /*= 0*/, uint32_t seqid /*= 0*/)
{
	return SendMsgToSvr(SVR_KIND_GAME, svr_id, pMsg, usProto, usModule, usCode, seqid);
}

GameSession* ModHttpSession::GetGame(uint32_t id)
{
	SvrKindID** p = _lstSes.GetID(SVR_KIND_GAME, id);
	if (p == nullptr) return nullptr;
	return dynamic_cast<GameSession*>(*p);
}

GateSession* ModHttpSession::GetGate(uint32_t id)
{
	SvrKindID** p = _lstSes.GetID(SVR_KIND_GATE, id);
	if (p == nullptr) return nullptr;
	return dynamic_cast<GateSession*>(*p);
}
