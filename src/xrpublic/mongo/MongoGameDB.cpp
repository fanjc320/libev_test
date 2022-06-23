#include "pch.h"
#include "MongoGameDB.h"

bool MongoGameDB::Init(MongoDb* pMongo)
{
	_mongo = pMongo;
	return true;
}
