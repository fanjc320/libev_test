#pragma once
#include "dtype.h"
#include "util/SingleTon.h"
#include "GatePlayer.h"

class GatePlayer;

class GatePlayerMgr : public SingleTon<GatePlayerMgr>
{
public:
	GatePlayer* Newplayer(int64_t accid);
	GatePlayer* GetPlayer(int64_t accid);

	void AddDeletePlayer(GatePlayer* pPlayer);
	void OnFrame(int64_t qwTm);
	void Fini();
protected:
	umap<int64_t, GatePlayer*> _mapPlayer;
	umap<int64_t, GatePlayer*> _mapDelPlayer;

};