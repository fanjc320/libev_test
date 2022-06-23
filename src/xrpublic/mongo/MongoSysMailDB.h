#pragma once
#include "dtype.h"
#include "db/mongo/MongoDb.h"

class MongoSysMailDB
{
public:
	bool Init(MongoDb* pMongo);

private:
	MongoDb* _mongo = nullptr;
};