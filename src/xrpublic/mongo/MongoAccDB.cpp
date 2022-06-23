#include "pch.h"
#include "MongoAccDB.h"

bool MongoAccDB::Init(MongoDb* pMongo)
{
	_mongo = pMongo;
	return true;
}
