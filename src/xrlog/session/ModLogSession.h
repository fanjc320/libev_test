#pragma once
#include "dtype.h"
#include "server/SvrModuleBase.h"
#include "util/SingleTon.h"
#include "server/mq/SvrProgMq.h"
#include "server/mq/SvrProgMqHandle.h"
#include "server/SvrKindList.h"

class GameSession;
class GateSession;

class ModLogSession :
    public SvrModuleBase, public SingleTon<ModLogSession>
{
public:
	virtual bool LogicInit() override;
	virtual void LogicFini() override;

	virtual bool Reload() override;
	virtual void OnFrame(uint64_t qwTm) override;
	virtual void OnTick(uint64_t qwTm) override; //“ª∫¡√Î“ª¥Œ

	GameSession* GetGame(uint32_t id);
	GateSession* GetGate(uint32_t id);

protected:
	SvrProgMqHandle _Handle;
	SvrKindList		_lstSes;
};

