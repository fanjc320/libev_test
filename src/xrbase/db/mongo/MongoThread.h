/*
 *
 *      Author: venture
 */
#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <functional>

#include "util/SingleTon.h"
#include "db/base/DbDef.h"
#include "db/base/DbThread.h"

class MongoDb;
class MongoThread : public DbThread
{
public:
	MongoThread(void);
	~MongoThread(void);

public:
	virtual bool Init(const std::string& hostname, const std::string& user
		, const std::string& pwd, const std::string& dbname, const std::string& auth,
		int32_t nAreanNo);
	virtual void MainLoop(time_t tmTick) override;
	virtual int64 Exec(DbRequest &oRequest) override;

private:
	MongoDb*	  _mongo = nullptr;
};
