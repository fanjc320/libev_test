#include "pch.h"
#include "MongoFriendDB.h"

bool MongoFriendDB::Init(MongoDb* pMongo)
{
	_mongo = pMongo;
	return true;
}
