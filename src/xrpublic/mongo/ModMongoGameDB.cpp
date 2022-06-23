#include "pch.h"
#include "ModMongoGameDB.h"
#include "config/ModConfig.h"
#include "config/ConfigStruct.h"
#include "log/LogMgr.h"

ModMongoGameDB::ModMongoGameDB()
{

}

bool ModMongoGameDB::LogicInit()
{
	bool ret = false;

	sMongoSql cfg = ModConfig::GetInstance()->GetConst().GetMongoConfig(_cfgId)._GameMongo;
	ERROR_LOG_EXIT0(cfg.IsValid());
	ret = _mongo.Init(cfg._host, cfg._user, cfg._pwd, cfg._name, cfg._auth);
	ERROR_LOG_EXIT0(ret);

	return SvrModuleBase::LogicInit();
Exit0:
	return false;
}

void ModMongoGameDB::LogicFini()
{
	_mongo.UnInit();
}

bool ModMongoGameDB::Reload()
{
	return true;
}

void ModMongoGameDB::OnFrame(uint64_t qwTm)
{
	_mongo.MainLoop(qwTm);
}
