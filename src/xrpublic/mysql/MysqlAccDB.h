#pragma once
#include "dtype.h"
#include "db/mysql/MysqlDb.h"

class MysqlAccDB
{
public:
	bool Init(MysqlDb* pMysql);
private:
	MysqlDb* _mysql = nullptr;
};