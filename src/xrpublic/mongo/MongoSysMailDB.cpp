#include "pch.h"
#include "MongoSysMailDB.h"

bool MongoSysMailDB::Init(MongoDb* pMongo)
{
	_mongo = pMongo;
	return true;
}
