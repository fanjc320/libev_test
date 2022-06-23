/*
 *
 *      Author: venture
 */
#pragma once

#include <string>
#include "db/base/DbDef.h"
#include "dtype.h"
#include <exception>
#include "db/mongo/MongoDef.h"
#include "time/time_func_x.h"

class MongoDbBase;
class MongoDb
{
public:
	MongoDb();
	~MongoDb();

    void UnInit();

    //auth:MONGODB-CR, SCRAM-SHA-1
    virtual bool Init(const std::string& hostname, const std::string& user
        , const std::string& pwd, const std::string& dbname, const std::string& auth
        , int32_t Areano = 0);

    bool MainLoop(time_t tmTick);

    MongoDbBase* GetMongoBase() { return _pMongo; }

public:
	bool DbExec(DbRequest& request);

    bool DBSelectOne(DbSelectReq &request, std::string* pRetJson = nullptr);
    //返回一级多个
    bool DBSelect(DbSelectReq &request, FUNC_DB_RET& fnCall, size_t& retNum);
    bool DBSelect(DbSelectReq &request, FUNC_MONGO_RET& fnCall, size_t& retNum);

    bool DBSelectMore(DbSelectReq &request, FUNC_DB_RET& fnCall, size_t& retNum, int64 qwBeginID, int32 limit);
    bool DBSelectMore(DbSelectReq &request, FUNC_MONGO_RET& fnCall, size_t& retNum, int64 qwBeginID, int32 limit);

    bool IsExsitlst(DbRequest &request);

    //一级列表计数
    int64 DbCount(DbSelectReq &request);

    //只针对数字型的字段增加数值
    bool DbIncValue(DbRequest &request, DBRet* pRet = nullptr);

    //删除整个集合，慎用！！！！
    bool DropCollection(const std::string& strCol);

    void SetErr(const std::string& err, int32 errorno, const std::string& strLog);
    void SetErr(const std::string& err, int32 errorno = 0);

protected:
    //慎用，用于更新所有人的数据状态的
    bool DbUpdate_Many(DbRequest &request);

    //只适用于关系型一级结构数据,字段为基础类型
    //不支持字段为复合结构,复合的操作自己特殊处理
    //--------------------------------------------------------------
	bool DbInsertLog(DbRequest &request);
    bool DbDelete(DbRequest &request);
    bool DbUpdate(DbRequest &request);

    bool DbInsertUpdate(DbRequest &request);
	
	//第二层为数据object
	bool DbUpdateObject(DbRequest &request);

	//删除单个行的列
	bool DbDropParam(DBReqBase &request);

    //--------------------------------------------------------------

    //适用于二级列表型数据,字段为基础类型
    //不支持字段为复合结构,复合的操作自己特殊处理
    //--------------------------------------------------------------
	bool DbInsertLogLst(DbRequest &request);
    bool DbInsertLst(DbRequest &request, bool *pMatch = nullptr);
    bool DbDeleteLst(DBReqBase &request);
    bool DbUpdateLst(DbRequest &request, bool *pMatch = nullptr);
	bool DbInsertUpdateLst(DbRequest &request, bool bUpFirst = true);

    //--------------------------------------------------------------
	void DoError(const std::string& funcName, int lineno);
    
    void AddQueryLog(const std::string& query);
    const char* GetQueryLog();

    const std::string& GetLastException() const { return _strException; }
    int32  GetErrorNo() const { return _errorno; }

    void ClearErr();
    void LogName(const std::string& tblName, const std::string& secTable = "");

private:
    MongoDbBase*        _pMongo = nullptr;
    int32               _errorno = 0;

    //错误记录   
    std::string         _strQueryLog;
    std::string		    _strException;

    //log用的，记录当前操作的数据表,不要在外面赋值
    std::string		    _tblName;
    std::string		    _secTblName;
	time_t  _tmLast = get_curr_time();

};

