#pragma once
#include "dtype.h"
#include "util/SingleTon.h"
#include "server/SvrModuleBase.h"
#include "db/mongo/MongoDb.h"

class ModMongoSysMailDB : public SvrModuleBase, public SingleTon<ModMongoSysMailDB>
{
	ModMongoSysMailDB();
	friend SingleTon;
public:
	virtual bool LogicInit() override;
	virtual void LogicFini() override;

	virtual bool Reload() override;
	virtual void OnFrame(uint64_t qwTm) override;
	void SetCfgId(uint32_t val) { _cfgId = val; }

	MongoDb& Mongo() { return _mongo; }
private:
	MongoDb		_mongo;
	uint32_t    _cfgId = 1;
};