/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "MysqlThread.h"
#include "MysqlDb.h"

MysqlThread::MysqlThread(void)
{
    if(_mysql == nullptr)
        _mysql = new MysqlDb();
}

MysqlThread::~MysqlThread(void)
{
    SAFE_DELETE(_mysql);
}

bool MysqlThread::Init(const std::string &host, const std::string &user
	, const std::string &password, const std::string &name, const int &port)
{
	if (!_mysql->Init(host, user, password, name, port))
		return false;

	return DbThread::Init();
}

void MysqlThread::MainLoop(time_t tmTick)
{
	_mysql->MainLoop(tmTick);
}

void MysqlThread::PushSQL(const std::string& strSql)
{
    DbRequest item;
    item.Init( eDBOptType::STRING, strSql);
    DbThread::PushData(item);
}

int64 MysqlThread::Exec(DbRequest &oRequest)
{
    if (oRequest.type() != eDBOptType::STRING)
        return _mysql->DbExec(oRequest);
    else
        return _mysql->DbExec(oRequest.tbl().c_str());
}
