#include "pch.h"
#include "ModMongoSysMailDB.h"
#include "config/ModConfig.h"
#include "config/ConfigStruct.h"
#include "log/LogMgr.h"

ModMongoSysMailDB::ModMongoSysMailDB()
{

}

bool ModMongoSysMailDB::LogicInit()
{
	bool ret = false;

	sMongoSql cfg = ModConfig::GetInstance()->GetConst().GetMongoConfig(_cfgId)._AccountMongo;
	ERROR_LOG_EXIT0(cfg.IsValid());
	ret = _mongo.Init(cfg._host, cfg._user, cfg._pwd, cfg._name, cfg._auth);
	ERROR_LOG_EXIT0(ret);

	return SvrModuleBase::LogicInit();
Exit0:
	return false;
}

void ModMongoSysMailDB::LogicFini()
{

}

bool ModMongoSysMailDB::Reload()
{
	return true;
}

void ModMongoSysMailDB::OnFrame(uint64_t qwTm)
{
	_mongo.MainLoop(qwTm);
}
