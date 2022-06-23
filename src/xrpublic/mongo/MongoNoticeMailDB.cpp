#include "pch.h"
#include "MongoNoticeMailDB.h"

bool MongoNoticeMailDB::Init(MongoDb* pMongo)
{
	_mongo = pMongo;
	return true;
}
