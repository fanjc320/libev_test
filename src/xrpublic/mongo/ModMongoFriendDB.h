#pragma once
#include "dtype.h"
#include "util/SingleTon.h"
#include "db/mongo/MongoDb.h"
#include "server/SvrModuleBase.h"

class ModMongoFriendDB : public SvrModuleBase, public SingleTon<ModMongoFriendDB>
{
	ModMongoFriendDB();
	friend SingleTon;
public:
	virtual bool LogicInit() override;
	virtual void LogicFini() override;

	virtual bool Reload() override;
	virtual void OnFrame(uint64_t qwTm) override;
	void SetCfgId(uint32_t val) { _cfgId = val; }

	MongoDb& Mongo() { return _mongo; }
private:
	MongoDb		 _mongo;
	uint32_t     _cfgId = 1;
};