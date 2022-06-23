/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "MongoThread.h"
#include "log/LogMgr.h"
#include "db/mongo/MongoDb.h"
using namespace std::chrono;

MongoThread::MongoThread(void)
{
	if(_mongo == nullptr)
		_mongo = new MongoDb();
}


MongoThread::~MongoThread(void)
{
	SAFE_DELETE(_mongo);
}

bool MongoThread::Init(const std::string& hostname, const std::string& user
	, const std::string& pwd, const std::string& dbname, const std::string& auth,
	int32_t nAreanNo)
{
	if (!_mongo->Init(hostname, user, pwd, dbname, auth, nAreanNo))
		return false;
	return DbThread::Init();
}

void MongoThread::MainLoop(time_t tmTick)
{
	_mongo->MainLoop(tmTick);
}

int64 MongoThread::Exec(DbRequest &oRequest)
{
    if(oRequest.type() != eDBOptType::NONE)
	    return _mongo->DbExec(oRequest);
    return 0;
}
