#include "pch.h"
#include "MysqlLogDB.h"

bool MysqlLogDB::Init(MysqlDb* pMysql)
{
	_mysql = pMysql;
	return true;
}

