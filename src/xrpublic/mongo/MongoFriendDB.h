#pragma once
#include "dtype.h"
#include "db/mongo/MongoDb.h"

class MongoFriendDB
{
public:
	bool Init(MongoDb* pMongo);

private:
	MongoDb* _mongo = nullptr;
};