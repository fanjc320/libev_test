#include "pch.h"
#include "ModConfigRedis.h"
#include "config/ModConfig.h"
#include "config/ConstConfig.h"

ModConfigRedis::ModConfigRedis() : SvrModuleBase("ModConfigRedis")
{

}

bool ModConfigRedis::LogicInit()
{
    bool ret = false;

    sIpPortRedis cfg = ModConfig::GetInstance()->GetConst().GetCacheRedis(_cfgId)._config;
    ERROR_LOG_EXIT0(cfg.IsValid());
    ret = _redis.Init(cfg._ip, cfg._port, cfg._pwd);
    ERROR_LOG_EXIT0(ret);

    return SvrModuleBase::LogicInit();
Exit0:
    return false;
}

void ModConfigRedis::LogicFini()
{
    _redis.UnInit();
}

bool ModConfigRedis::Reload()
{
    return SvrModuleBase::Reload();
}

void ModConfigRedis::OnFrame(uint64_t qwTm)
{
    _redis.MainLoop(get_curr_time());
}
