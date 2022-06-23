#include "pch.h"
#include "ModMongoMallDB.h"
#include "config/ModConfig.h"
#include "config/ConfigStruct.h"
#include "log/LogMgr.h"

ModMongoMallDB::ModMongoMallDB()
{

}

bool ModMongoMallDB::LogicInit()
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

void ModMongoMallDB::LogicFini()
{

}

bool ModMongoMallDB::Reload()
{
	return true;
}

void ModMongoMallDB::OnFrame(uint64_t qwTm)
{
	_mongo.MainLoop(qwTm);
}
