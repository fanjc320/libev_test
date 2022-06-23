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
    //����һ�����
    bool DBSelect(DbSelectReq &request, FUNC_DB_RET& fnCall, size_t& retNum);
    bool DBSelect(DbSelectReq &request, FUNC_MONGO_RET& fnCall, size_t& retNum);

    bool DBSelectMore(DbSelectReq &request, FUNC_DB_RET& fnCall, size_t& retNum, int64 qwBeginID, int32 limit);
    bool DBSelectMore(DbSelectReq &request, FUNC_MONGO_RET& fnCall, size_t& retNum, int64 qwBeginID, int32 limit);

    bool IsExsitlst(DbRequest &request);

    //һ���б����
    int64 DbCount(DbSelectReq &request);

    //ֻ��������͵��ֶ�������ֵ
    bool DbIncValue(DbRequest &request, DBRet* pRet = nullptr);

    //ɾ���������ϣ����ã�������
    bool DropCollection(const std::string& strCol);

    void SetErr(const std::string& err, int32 errorno, const std::string& strLog);
    void SetErr(const std::string& err, int32 errorno = 0);

protected:
    //���ã����ڸ��������˵�����״̬��
    bool DbUpdate_Many(DbRequest &request);

    //ֻ�����ڹ�ϵ��һ���ṹ����,�ֶ�Ϊ��������
    //��֧���ֶ�Ϊ���Ͻṹ,���ϵĲ����Լ����⴦��
    //--------------------------------------------------------------
	bool DbInsertLog(DbRequest &request);
    bool DbDelete(DbRequest &request);
    bool DbUpdate(DbRequest &request);

    bool DbInsertUpdate(DbRequest &request);
	
	//�ڶ���Ϊ����object
	bool DbUpdateObject(DbRequest &request);

	//ɾ�������е���
	bool DbDropParam(DBReqBase &request);

    //--------------------------------------------------------------

    //�����ڶ����б�������,�ֶ�Ϊ��������
    //��֧���ֶ�Ϊ���Ͻṹ,���ϵĲ����Լ����⴦��
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

    //�����¼   
    std::string         _strQueryLog;
    std::string		    _strException;

    //log�õģ���¼��ǰ���������ݱ�,��Ҫ�����渳ֵ
    std::string		    _tblName;
    std::string		    _secTblName;
	time_t  _tmLast = get_curr_time();

};

