#include "pch.h"
#include "GatePlayerMgr.h"

GatePlayer* GatePlayerMgr::Newplayer(int64_t accid)
{
	GatePlayer* pPlayer = new GatePlayer(accid);
	if (pPlayer == NULL) return NULL;
	_mapPlayer[accid] = pPlayer;
	return pPlayer;
Exit0:
	return NULL;
}

GatePlayer* GatePlayerMgr::GetPlayer(int64_t accid)
{
	MAP_FIND_NULL(_mapPlayer, accid);
}

void GatePlayerMgr::AddDeletePlayer(GatePlayer* pPlayer)
{
	_mapDelPlayer[pPlayer->GetAccId()] = pPlayer;
}

void GatePlayerMgr::OnFrame(int64_t qwTm)
{
	for (auto& itr : _mapDelPlayer)
	{
		_mapPlayer.erase(itr.first);
	}
	_mapDelPlayer.clear();

}

void GatePlayerMgr::Fini()
{

}