/*
 *
 *      Author: venture
 */
#pragma once

#include <string>
#include "db/base/DbDef.h"
#include "time/time_func_x.h"

using namespace std;
namespace mysqlpp
{
    class Connection;
}

class MysqlDb
{
public:
    MysqlDb();
    MysqlDb(const string& host, const string& user
        , const string& password, const string& name, int& port);
    ~MysqlDb();

public:
    bool Init(const string& host, const string& user
        , const string& password, const string& name, const int& port);
    void UnInit();

    void SetDbName(const string& name) { _strName = name; }

    bool DbConnect();
    void DbDisConnect();
    bool IsConnected();

    bool DBSelect_One(DbSelectReq& request);

    //查询，或者执行存储过程调用
    bool DBSelect(DbSelectReq& request, FUNC_DB_MAP_RET& fnCall, size_t& retNum);
    bool DBSelect(const std::string& sql, FUNC_DB_MAP_RET& fnCall, size_t& retNum);
    //执行存储过程调用
    bool DbCallProc(const std::string& sql);

    //无返回值的操作
    int64   DbExec(const std::string& sql, eDBOptType queryType = eDBOptType::NONE);
    int64   DbExec(DbRequest& request);
    bool    DbExec(std::list<std::string>& vecCreateSql);
    bool   DbInsert(DbRequest& request);
    bool   DbDelete(DbRequest& request);
    bool   DbUpdate(DbRequest& request);
    bool   DbIncValue(DbRequest& request);
    bool   DbInsertUpdate(DbRequest& request);

    bool    MakeSqlStr(DbRequest& request);
    const std::string& GetSqlStr();

    bool    MainLoop(time_t tmTick);
    bool    DbPing();

    string GetHostName() { return _strHost; };
    string GetUser() { return _strUser; };
    string GetPwd() { return _strPwd; };
    string GetName() { return _strName; };
    uint32 GetPort() { return _dwPort; };

    bool UpdateDBFile(const char* pszFileName, const char* pszDBName
        , int nDBId = 1, const char* pszTableName = NULL);

    bool UpdateLogFile(const char* pszFileName, const std::string& strDate);

private:
    mysqlpp::Connection* _pMysql = nullptr;
    string _szQuery;

    string _strHost;
    string _strUser;
    string _strPwd;
    string _strName;
    uint32 _dwPort = 0;
    uint32 _dwReconTimes = 0;
    time_t _tmLast = get_curr_time();

private:
    bool _DBSelect(const std::string& sql, FUNC_DB_MAP_RET& fnCall, DBMap& ret, size_t& retNum, bool bOne);

    const char* _GetDBLastError();
    const string& _GetLastQuery();

    string _MakeInsert(DBMap& mapData);
    string _MakeInsert(DBMap& mapCon, DBMap& mapReq);

    string _MakeUpdate(DBMap& mapData);

    string _MakeIncInsert(DBMap& mapCon, DBMap& mapReq);
    string _MakeInc(DBMap& mapData);
    string _MakeFields(uset<std::string>& vFields);

    string _MakeCon(DBMap& condition);
    void   _MakeOpType(std::string& ss, const string& skey, const string& sValue, eDBEQType eType = eDBEQType::EQ);
    void   _MakeOpType(std::string& ss, const string& skey, const int64& sValue, eDBEQType eType = eDBEQType::EQ);

    bool _OpenCreateDBFile(const string& strFileName, std::list<std::string>& vecCreateSql
        , int nDBId, const string& pszTableName);
    bool _OpenLogDBFile(const string& strFileName, std::list<std::string>& vecCreateSql, const string& strDate);

    bool _IsNetError(int nErr);

    bool _StreamItem(bool& isFirst, std::string& szName, std::string& szValue, umap < std::string, DBMap::DBItemNum>& mapData);
    bool _StreamItem(bool& isFirst, std::string& szName, std::string& szValue, umap< std::string, DBMap::DBItemStr>& mapData);
    bool _StreamItem(bool& isFirst, std::string& szName, std::string& szValue, umap< std::string, double>& mapData);
};