#pragma once
#include "dtype.h"
#include "server/SvrModuleBase.h"
#include "util/SingleTon.h"
#include "redis/GameMqRedis.h"

class ModMqRedis : public SvrModuleBase, public SingleTon<ModMqRedis>
{
    ModMqRedis();
    friend SingleTon;
public:
    virtual bool LogicInit() override;
    virtual void LogicFini() override;

    virtual bool Reload() override;
	virtual void OnFrame(uint64_t qwTm) override;

    GameMqRedis& Redis() { return _redis; }
    void SetCfgId(uint32_t val) { _cfgId = val; }
private:
    GameMqRedis _redis;
    uint32_t    _cfgId = 1;
};