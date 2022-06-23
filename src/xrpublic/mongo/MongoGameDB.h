#pragma once
#include "dtype.h"
#include "db/mongo/MongoDb.h"

class MongoGameDB
{
public:
	bool Init(MongoDb* pMongo);

private:
	MongoDb* _mongo = nullptr;
};