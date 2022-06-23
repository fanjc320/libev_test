#include "pch.h"
#include "MongoMallDB.h"

bool MongoMallDB::Init(MongoDb* pMongo)
{
	_mongo = pMongo;
	return true;
}
