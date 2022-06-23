/*
 *
 *      Author: venture
 */
#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include "platform/spthread.h"
#include "util/SingleTon.h"
#include "db/base/DbDef.h"
#include "db/base/DbThread.h"

class MysqlDb;
class MysqlThread : public DbThread
{
public:
	MysqlThread(void);
	~MysqlThread(void);

public:
	virtual bool Init(const std::string &host, const std::string &user
		, const std::string &password, const std::string &name, const int &port);

    virtual void PushSQL(const std::string& strSql);

	virtual void MainLoop(time_t tmTick) override;
	virtual int64 Exec(DbRequest &oRequest) override;

private:
	MysqlDb*	_mysql = nullptr;
};