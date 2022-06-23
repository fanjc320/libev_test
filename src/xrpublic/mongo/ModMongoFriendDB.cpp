#include "pch.h"

#include "ModMongoFriendDB.h"
#include "config/ModConfig.h"
#include "config/ConfigStruct.h"
#include "log/LogMgr.h"

ModMongoFriendDB::ModMongoFriendDB()
{

}

bool ModMongoFriendDB::LogicInit()
{
	bool ret = false;

	sMongoSql cfg = ModConfig::GetInstance()->GetConst().GetMongoConfig(_cfgId)._FriendMongo;
	ERROR_LOG_EXIT0(cfg.IsValid());
	ret = _mongo.Init(cfg._host, cfg._user, cfg._pwd, cfg._name, cfg._auth);
	ERROR_LOG_EXIT0(ret);

	return SvrModuleBase::LogicInit();
Exit0:
	return false;
}

void ModMongoFriendDB::LogicFini()
{
	_mongo.UnInit();
}

bool ModMongoFriendDB::Reload()
{
	return true;
}

void ModMongoFriendDB::OnFrame(uint64_t qwTm)
{
	_mongo.MainLoop(qwTm);
}
