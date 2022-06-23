#include "pch.h"
#include "ModMqRedis.h"
#include "config/ModConfig.h"
#include "config/ConstConfig.h"

ModMqRedis::ModMqRedis() : SvrModuleBase("ModMqRedis")
{

}

bool ModMqRedis::LogicInit()
{
	bool ret = false;

	sIpPortRedis cfg = ModConfig::GetInstance()->GetConst().GetCacheRedis(_cfgId)._mq;
	ERROR_LOG_EXIT0(cfg.IsValid());
	ret = _redis.Init(cfg._ip, cfg._port, cfg._pwd);
	ERROR_LOG_EXIT0(ret);

	return SvrModuleBase::LogicInit();
Exit0:
	return false;
}

void ModMqRedis::LogicFini()
{
	_redis.UnInit();
}

bool ModMqRedis::Reload()
{
    return SvrModuleBase::Reload();
}

void ModMqRedis::OnFrame(uint64_t qwTm)
{
	_redis.MainLoop(get_curr_time());
}
