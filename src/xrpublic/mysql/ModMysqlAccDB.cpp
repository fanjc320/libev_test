#include "pch.h"
#include "mysql/ModMysqlAccDB.h"
#include "mysql/MysqlLogDB.h"
#include "config/ModConfig.h"
#include "config/ConfigStruct.h"
#include "log/LogMgr.h"

ModMysqlAccDB::ModMysqlAccDB()
{

}

bool ModMysqlAccDB::LogicInit()
{
	bool ret = false;

	sMySql cfg = ModConfig::GetInstance()->GetConst().GetMySqlConfig(_cfgId)._AccMySql;
	ERROR_LOG_EXIT0(cfg.IsValid());
	ret = _mysql.Init(cfg._host, cfg._user, cfg._pwd, cfg._name, cfg._port);
	ERROR_LOG_EXIT0(ret);

	return SvrModuleBase::LogicInit();
Exit0:
	return false;
}

void ModMysqlAccDB::LogicFini()
{
	_mysql.UnInit();
}

bool ModMysqlAccDB::Reload()
{
	return true;
}

void ModMysqlAccDB::OnFrame(uint64_t qwTm)
{
	_mysql.MainLoop(get_curr_time());
}
