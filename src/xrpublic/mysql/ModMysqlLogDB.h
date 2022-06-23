#pragma once
#include "dtype.h"
#include "util/SingleTon.h"
#include "server/SvrModuleBase.h"
#include "db/mysql/MysqlDb.h"

class ModMysqlLogDB : public SvrModuleBase, public SingleTon<ModMysqlLogDB>
{
	ModMysqlLogDB();
	friend SingleTon;
public:
	virtual bool LogicInit() override;
	virtual void LogicFini() override;

	virtual bool Reload() override;
	virtual void OnFrame(uint64_t qwTm) override;
	void SetCfgId(uint32_t val) { _cfgId = val; }

	MysqlDb& Mysql() {return _mysql; }
private:
	MysqlDb		_mysql;
	uint32_t    _cfgId = 1;
};