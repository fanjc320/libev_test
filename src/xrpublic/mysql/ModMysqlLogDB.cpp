#include "pch.h"
#include "mysql/ModMysqlLogDB.h"
#include "mysql/MysqlLogDB.h"
#include "config/ModConfig.h"
#include "config/ConfigStruct.h"
#include "log/LogMgr.h"

ModMysqlLogDB::ModMysqlLogDB()
{

}

bool ModMysqlLogDB::LogicInit()
{
	bool ret = false;

	sMySql cfg = ModConfig::GetInstance()->GetConst().GetMySqlConfig(_cfgId)._LogMySql;
	ERROR_LOG_EXIT0(cfg.IsValid());
	ret = _mysql.Init(cfg._host, cfg._user, cfg._pwd, cfg._name, cfg._port);
	ERROR_LOG_EXIT0(ret);

	return SvrModuleBase::LogicInit();
Exit0:
	return false;
}

void ModMysqlLogDB::LogicFini()
{
	_mysql.UnInit();
}

bool ModMysqlLogDB::Reload()
{
	return true;
}

void ModMysqlLogDB::OnFrame(uint64_t qwTm)
{
	_mysql.MainLoop(get_curr_time());
}
