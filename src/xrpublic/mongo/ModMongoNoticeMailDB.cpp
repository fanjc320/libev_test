#include "pch.h"
#include "ModMongoNoticeMailDB.h"
#include "config/ModConfig.h"
#include "config/ConfigStruct.h"
#include "log/LogMgr.h"

ModMongoNoticeMailDB::ModMongoNoticeMailDB()
{

}

bool ModMongoNoticeMailDB::LogicInit()
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

void ModMongoNoticeMailDB::LogicFini()
{

}

bool ModMongoNoticeMailDB::Reload()
{
	return true;
}

void ModMongoNoticeMailDB::OnFrame(uint64_t qwTm)
{
	_mongo.MainLoop(qwTm);
}
