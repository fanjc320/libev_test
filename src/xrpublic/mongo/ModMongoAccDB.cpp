#include "pch.h"
#include "ModMongoAccDB.h"
#include "config/ModConfig.h"
#include "config/ConfigStruct.h"
#include "log/LogMgr.h"

ModMongoAccDB::ModMongoAccDB()
{

}

bool ModMongoAccDB::LogicInit()
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

void ModMongoAccDB::LogicFini()
{
	_mongo.UnInit();
}

bool ModMongoAccDB::Reload()
{
	return true;
}

void ModMongoAccDB::OnFrame(uint64_t qwTm)
{
	_mongo.MainLoop(qwTm);
}
