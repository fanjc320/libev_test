#pragma once
#include "dtype.h"
#include "server/SvrModuleBase.h"
#include "util/SingleTon.h"
#include "redis/GameConfigRedis.h"

class ModConfigRedis : public SvrModuleBase, public SingleTon<ModConfigRedis>
{
    ModConfigRedis();
    friend SingleTon;
public:
    virtual bool LogicInit() override;
    virtual void LogicFini() override;

    virtual bool Reload() override;
    virtual void OnFrame(uint64_t qwTm) override;
	void SetCfgId(uint32_t val) { _cfgId = val; }

    GameConfigRedis& Redis() { return _redis; }
private:
    GameConfigRedis _redis;
	uint32_t    _cfgId = 1;
};