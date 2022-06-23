#include "pch.h"
#include "MysqlAccDB.h"

bool MysqlAccDB::Init(MysqlDb* pMysql)
{
	_mysql = pMysql;
	return true;
}

