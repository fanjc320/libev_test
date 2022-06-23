#include "pch.h"
#include "ModLogSession.h"
#include "GameSession.h"
#include "GateSession.h"
#include "server/SvrAppBase.h"

bool ModLogSession::LogicInit()
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

void ModLogSession::LogicFini()
{
	_Handle.FreeReconT();
	_lstSes.Clear();
}

bool ModLogSession::Reload()
{
	return SvrModuleBase::Reload();
}

void ModLogSession::OnFrame(uint64_t qwTm)
{
	for (auto& itr : _Handle.GetMap())
	{
		itr->OnFrame(qwTm);
	}
}

void ModLogSession::OnTick(uint64_t qwTm)
{
	for (auto& itr : _Handle.GetMap())
	{
		itr->OnTick(qwTm);
	}
}

GameSession* ModLogSession::GetGame(uint32_t id)
{
	SvrKindID** p = _lstSes.GetID(SVR_KIND_GAME, id);
	if (p == nullptr) return nullptr;
	return dynamic_cast<GameSession*>(*p);
}

GateSession* ModLogSession::GetGate(uint32_t id)
{
	SvrKindID** p = _lstSes.GetID(SVR_KIND_GATE, id);
	if (p == nullptr) return nullptr;
	return dynamic_cast<GateSession*>(*p);
}
