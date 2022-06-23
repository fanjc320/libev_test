/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <typeinfo>

#include "MongoDb.h"
#include "db/base/DbDef.h"
#include "log/LogMgr.h"
#include "time/TimeDateX.h"
#include "time/time_func_x.h"
#include <math.h>
#include <chrono>
#include "mongocxx/exception/query_exception.hpp"
#include "mongocxx/exception/operation_exception.hpp"

#include <bsoncxx/types.hpp>
#include "db/mongo/MongoDbBase.h"
#include "db/mongo/MongoDbField.h"
#include "db/mongo/MongoDef.h"

MongoDb::MongoDb()
{
    if (_pMongo == nullptr)
        _pMongo = new MongoDbBase();
}

MongoDb::~MongoDb()
{
    SAFE_DELETE(_pMongo);
}

void MongoDb::UnInit()
{
    _pMongo->UnInit();
}

bool MongoDb::Init(const std::string& hostname, const std::string& user, const std::string& pwd, const std::string& dbname, const std::string& auth, int32_t Areano)
{
    return _pMongo->Init(hostname, user, pwd, dbname, auth, Areano);
}

bool MongoDb::DbExec(DbRequest& request)
{
    if (request.type() != eDBOptType::UPDATE_MANY && request.type() != eDBOptType::DROP
        && !request.CheckKey())
    {
        LOG_ERROR("key error! %s:%s type:%d, class:%s", request.tbl().c_str()
            , request.sectbl().name().c_str(), request.type(), typeid(*this).name());
        return false;
    }

	bool bRet = true;
	eDBSecKeyType eSecType = request.sectbl().type();
	switch (request.type())
	{
	case eDBOptType::INSERT:
        if (eSecType == eDBSecKeyType::LIST)
			bRet = DbInsertUpdateLst(request, false);
		else if(eSecType == eDBSecKeyType::NONE)
			bRet = DbUpdate(request);//修改为update
		else if (eSecType == eDBSecKeyType::OBJECT)
			bRet = DbUpdateObject(request);
		break;
	case eDBOptType::UPDATE:
		if (eSecType == eDBSecKeyType::LIST)
			bRet = DbInsertUpdateLst(request);
		else if (eSecType == eDBSecKeyType::NONE)
			bRet = DbUpdate(request);
		else if (eSecType == eDBSecKeyType::OBJECT)
			bRet = DbUpdateObject(request);
		break;
	case eDBOptType::INSERT_UPDATE:
        if (eSecType == eDBSecKeyType::LIST)
            bRet = DbInsertUpdateLst(request);
		else if (eSecType == eDBSecKeyType::NONE)
			bRet = DbInsertUpdate(request);
		else if (eSecType == eDBSecKeyType::OBJECT)
			bRet = DbUpdateObject(request);
		break;
	case eDBOptType::DELETE_X:
		if (eSecType == eDBSecKeyType::LIST)
			bRet = DbDeleteLst(request);
		else if (eSecType == eDBSecKeyType::NONE)
			bRet = DbDelete(request);
		break;
	case eDBOptType::INC:
		bRet = DbIncValue(request);
		break;
    case eDBOptType::INSERT_LOG:
        if (eSecType == eDBSecKeyType::NONE)
            bRet = DbInsertLog(request);
        else
		    bRet = DbInsertLogLst(request);
		break;
	case eDBOptType::DROP_DARAM:
		bRet = DbDropParam(request);
		break;
    case eDBOptType::UPDATE_MANY:
        bRet = DbUpdate_Many(request);
        break;
    case eDBOptType::DROP:
        bRet = DropCollection(request.tbl());
        break;
	default:
		LOG_ERROR("type error! %s:%s type:%d, class:%s", request.tbl().c_str()
			, request.sectbl().name().c_str(), request.type(), typeid(*this).name());
		return false;
	}

    if (!bRet)
    {
        LOG_ERROR("ret error! %s:%s type:%d, class:%s", request.tbl().c_str()
            , request.sectbl().name().c_str(), request.type(), typeid(*this).name());
    }

	return bRet;
}

bool MongoDb::IsExsitlst(DbRequest &request)
{
    if (!request.CheckKey())
    {
        LOG_ERROR("key error! %s:%s type:%d, class:%s", request.tbl().c_str()
            , request.sectbl().name().c_str(), request.type(), typeid(*this).name());
        return false;
    }

	LogName(request.tbl(), request.sectbl().name());
    stream::document jFind, jFields;

	BEGIN_TRY_MD
	{
		std::string strIndex = request.sectbl().name() + "." + request.S_KEY;
        if (!MongoDbField::MakeKeyBson(request.Con(), request.S_KEY, strIndex, jFind))
        {
            if (!MongoDbField::MakeKeyBson(request.reqMap(), request.S_KEY, strIndex, jFind))
            {
                LOG_ERROR("%s-%s: id is null", request.tbl().c_str()
                    , request.sectbl().name().c_str());
                return false;
            }
        }

        if (!MongoDbField::MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, jFind))
        {
            LOG_ERROR("%s-%s: _id is null", request.tbl().c_str()
                , request.sectbl().name().c_str());
            return false;
        }

        std::string fields = request.sectbl().name() + ".$";
		jFields << fields << true;

	    return _pMongo->_IsExsit(request.tbl(), jFind, &jFields);
	}
	ENG_TRY_MD(jFind, jFields);
	return false;
}

bool MongoDb::DbUpdate(DbRequest &request)
{
	LogName(request.tbl(), request.sectbl().name());
    stream::document jbuild, qer;

	BEGIN_TRY_MD
    {
        if (!MongoDbField::MakeCon(request, qer)) return false;
        MongoDbField::MakeUpdate(request, jbuild);
        bool bRet = _pMongo->_UpdateOne(request.tbl(), qer, jbuild, true);
        jbuild.clear();
        qer.clear();

        return bRet;
    }
	ENG_TRY_MD(qer, jbuild);
    return false;
}


bool MongoDb::DbUpdate_Many(DbRequest &request)
{
    LogName(request.tbl(), request.sectbl().name());
    stream::document jbuild, qer;

    BEGIN_TRY_MD
    {
        MongoDbField::MakeConMany(request, qer);
        if(request.sectbl().IsEmpty())
            MongoDbField::MakeUpdate(request, jbuild);
        else if(request.sectbl().IsList())
            MongoDbField::MakeUpdateLst(request, jbuild);
        else
            MongoDbField::MakeUpdateObject(request, jbuild);

        return _pMongo->_UpdateMany(request.tbl(), qer, jbuild, false);
    }
    ENG_TRY_MD(qer, jbuild);
    return false;
}

bool MongoDb::DBSelectOne(DbSelectReq &request, std::string* pRetJson)
{
    request.clear_ret();
    if (!request.CheckKey((int)eCheckKeyType::KEY))
    {
        LOG_ERROR("key error! %s:%s type:%d, class:%s", request.tbl().c_str()
            , request.sectbl().name().c_str(), request.type(), typeid(*this).name());
        return false;
    }
	LogName(request.tbl(), request.sectbl().name());
    stream::document jFields, qer;

	BEGIN_TRY_MD
    {
        if (!MongoDbField::MakeCon(request, qer)) return false;
        MongoDbField::MakeBsonFields(request, jFields);
        optx op = _pMongo->_FindOne(request.tbl(), qer, &jFields);
        if (op == bsoncxx::stdx::nullopt) return true;
        if (pRetJson != nullptr) *pRetJson = bsoncxx::to_json(op.value().view());
		return MongoDbField::SelectOne(request.Ret(), op.value().view());
	}
	ENG_TRY_MD(qer, jFields);
    return false;
}

bool MongoDb::DBSelect(DbSelectReq &request, FUNC_DB_RET& fnCall, size_t& retNum)
{
    retNum = 0;
	LogName(request.tbl(), request.sectbl().name());
    stream::document jFields, qer;

	BEGIN_TRY_MD
    {        
        size_t lstSize = MongoDbField::MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer);
        if (!MongoDbField::MakeCon(request, qer)) return false;
        MongoDbField::MakeBsonFields(request, jFields);

        if (lstSize >= 1)
        {
            mongocxx::cursor curs = _pMongo->_FindN(request.tbl(), qer, 0, 0, &jFields);
            for (const bsoncxx::document::view& bObj : curs)
            {
                MongoDbField::SelectOne(request.Ret(), bObj);
                if (!fnCall(request.Ret()))
                    return false;

                ++retNum;
            }
            return true;
        }
        else
        {
            LOG_ERROR("must have _id fields!, else use DBSelectMore!!");
            return false;
        }
	}
	ENG_TRY_MD(qer, jFields);
    return false;
}

bool MongoDb::DBSelect(DbSelectReq &request, FUNC_MONGO_RET& fnCall, size_t& retNum)
{
    retNum = 0;
    LogName(request.tbl(), request.sectbl().name());
    stream::document jFields, qer;

    BEGIN_TRY_MD
    {
        size_t lstSize = MongoDbField::MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer);
        if (!MongoDbField::MakeCon(request, qer)) return false;
        MongoDbField::MakeBsonFields(request, jFields);

        if (lstSize >= 1)
        {
            mongocxx::cursor curs = _pMongo->_FindN(request.tbl(), qer, 0, 0, &jFields);
            for (const bsoncxx::document::view& bObj : curs)
            {
                if (!fnCall(&bObj))
                    return false;

                ++retNum;
            }
            return true;
        }
        else
        {
            LOG_ERROR("must have _id fields!, else use DBSelectMore!!");
            return false;
        }
    }
    ENG_TRY_MD(qer, jFields);
    return false;
}

bool MongoDb::DBSelectMore(DbSelectReq &request, FUNC_DB_RET& fnCall
    , size_t& retNum, int64 qwBeginID, int32 limit)
{
    retNum = 0;
    LogName(request.tbl(), request.sectbl().name());
    stream::document jFields, qer;

    BEGIN_TRY_MD
    {
        size_t lstSize = MongoDbField::MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer);
        if (!MongoDbField::MakeCon(request, qer)) return false;
        MongoDbField::MakeBsonFields(request, jFields);

        if (lstSize >= 1)
        {
            mongocxx::cursor curs = _pMongo->_FindN(request.tbl(), qer, limit, 0, &jFields);
            for (const bsoncxx::document::view& bObj : curs)
            {
                MongoDbField::SelectOne(request.Ret(), bObj);
                if (!fnCall(request.Ret()))
                    return false;

                ++retNum;
            }
            return true;
        }
        else
       {
            stream::document dSort;
            MongoDbField::AppendNum(dSort, request.F_KEY, 1, eDBEQType::EQ);
            MongoDbField::AppendNum(qer, request.F_KEY, qwBeginID, eDBEQType::GT);

            mongocxx::cursor curs = _pMongo->_FindN(request.tbl(), qer, limit, 0, &jFields, &dSort);
            for (const bsoncxx::document::view& bObj : curs)
            {
                MongoDbField::SelectOne(request.Ret(), bObj);
                if (!fnCall(request.Ret()))
                    return false;

                ++retNum;
            }
            return true;
        }
    }
    ENG_TRY_MD(qer, jFields);
    return false;
}

bool MongoDb::DBSelectMore(DbSelectReq &request, FUNC_MONGO_RET& fnCall, size_t& retNum, int64 qwBeginID, int32 limit)
{
    retNum = 0;
    LogName(request.tbl(), request.sectbl().name());
    stream::document jFields, qer;

    BEGIN_TRY_MD
    {
        size_t lstSize = MongoDbField::MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer);
        if (!MongoDbField::MakeCon(request, qer)) return false;
        MongoDbField::MakeBsonFields(request, jFields);

        if (lstSize >= 1)
        {
            mongocxx::cursor curs = _pMongo->_FindN(request.tbl(), qer, limit, 0, &jFields);
            for (const bsoncxx::document::view& bObj : curs)
            {
                if (!fnCall(&bObj))
                    return false;

                ++retNum;
            }
            return true;
        }
        else
        {
            stream::document dSort;
            MongoDbField::AppendNum(dSort, request.F_KEY, 1, eDBEQType::EQ);
            MongoDbField::AppendNum(qer, request.F_KEY, qwBeginID, eDBEQType::GT);

            mongocxx::cursor curs = _pMongo->_FindN(request.tbl(), qer, limit, 0, &jFields, &dSort);
            for (const bsoncxx::document::view& bObj : curs)
            {
                if (!fnCall(&bObj))
                    return false;

                ++retNum;
            }
            return true;
        }
    }
    ENG_TRY_MD(qer, jFields);
    return false;
}


int64 MongoDb::DbCount(DbSelectReq &request)
{
	LogName(request.tbl(), request.sectbl().name());
    stream::document qer;

	BEGIN_TRY_MD
	{
        if (!MongoDbField::MakeCon(request, qer)) return false;

		return (int64)_pMongo->_Count(request.tbl(), qer);
	}
	ENG_TRY_MD(qer, qer);
	return 0;
}

bool MongoDb::DbInsertLog(DbRequest &request)
{
	LogName(request.tbl(), request.sectbl().name());
    stream::document jbuild;

	BEGIN_TRY_MD
	{
        MongoDbField::MakeBsonMap(request.reqMap(), jbuild);
		bool bRet = _pMongo->_Insert(request.tbl(), jbuild);
        jbuild.clear();
        return bRet;
	}
	ENG_TRY_MD(jbuild, jbuild);
    return false;
}

bool MongoDb::DbDelete(DbRequest &request)
{
	LogName(request.tbl(), request.sectbl().name());
    stream::document qer;

	BEGIN_TRY_MD
    {   
        bool bMul = false;
        if (!MongoDbField::MakeConDelete(request, qer, bMul))
            return false;

        if(bMul)
            return _pMongo->_Remove(request.tbl(), qer, false, request.IsDelError());
        else
            return _pMongo->_Remove(request.tbl(), qer, true, request.IsDelError());
    }
	ENG_TRY_MD(qer, qer);
    return false;
}

bool MongoDb::DbUpdateObject(DbRequest &request)
{
	LogName(request.tbl(), request.sectbl().name());
    stream::document jbuild, qer;

	BEGIN_TRY_MD
	{
        if (!MongoDbField::MakeCon(request, qer)) return false;
        MongoDbField::MakeUpdateObject(request, jbuild);
		return _pMongo->_UpdateOne(request.tbl(), qer, jbuild, true);
	}
	ENG_TRY_MD(qer, jbuild);
	return false;
}

bool MongoDb::DbDropParam(DBReqBase &request)
{
	LogName(request.tbl(), request.sectbl().name());
    stream::document qer, jret;

	BEGIN_TRY_MD
	{
        if (!MongoDbField::MakeCon(request, qer)) return false;
		
		//设置字段
		stream::document jadd;
		for (const auto &itr : request.Fields())
		{
			jadd << itr << true;
		}
		jret << "$unset" << jadd;
		return _pMongo->_UpdateOne(request.tbl(), qer, jret, true);
	}
	ENG_TRY_MD(qer, jret);
	return false;
}

bool MongoDb::DbInsertUpdate(DbRequest &request)
{
	LogName(request.tbl(), request.sectbl().name());
    stream::document jbuild, qer;

	BEGIN_TRY_MD
    {
        if (!MongoDbField::MakeCon(request, qer)) return false;
        MongoDbField::MakeUpdate(request, jbuild);
        return _pMongo->_UpdateOne(request.tbl(), qer, jbuild, true);
    }
	ENG_TRY_MD(qer, jbuild);
    return false;
}

bool MongoDb::DbIncValue(DbRequest &request, DBRet* pRet)
{
    if (!request.CheckKey())
    {
        LOG_ERROR("key error! %s:%s type:%d, class:%s", request.tbl().c_str()
            , request.sectbl().name().c_str(), request.type(), typeid(*this).name());
        return false;
    }

    if (pRet != nullptr)
        pRet->clear();

	LogName(request.tbl(), request.sectbl().name());
    stream::document qer, jret;

	BEGIN_TRY_MD
	{
        if (!MongoDbField::MakeCon(request, qer)) return false;
		//自增单个字段
		stream::document jadd;

        switch (request.sectbl().type())
        {
        case eDBSecKeyType::LIST:
            MongoDbField::MakeBson(request.reqMap().MapNum(), jadd, request.sectbl().name()+".$.");
            break;
        case eDBSecKeyType::OBJECT:
            MongoDbField::MakeBson(request.reqMap().MapNum(), jadd, request.sectbl().name()+".");
            break;
        case eDBSecKeyType::NONE:
            MongoDbField::MakeBson(request.reqMap().MapNum(), jadd);
            break;
        default:
            LOG_ERROR("sectype is error!, %s-%s"
                , request.tbl().c_str(), request.sectbl().name().c_str());
            return false;
        }

		jret << "$inc" << jadd;

		if (pRet != nullptr)
		{
			optx bsQer = _pMongo->_FindAndModify(request.tbl(), qer, jret, true);
            if(bsQer != bsoncxx::stdx::nullopt)
			   return MongoDbField::SelectOne(*pRet, bsQer.value().view());
		}
		else
			return _pMongo->_UpdateOne(request.tbl(), qer, jret, true);
	}
	ENG_TRY_MD(qer, jret);
	return false;
}

bool MongoDb::DropCollection(const std::string& strCol)
{
    LOG_CUSTOM("dbdrop", "table: %s", strCol.c_str());
    if (strCol.empty()) return true;

    LogName(strCol);
    stream::document qer, jIns;
    BEGIN_TRY_MD
    {
        return _pMongo->_DropCollection(strCol);
    }
    ENG_TRY_MD(qer, jIns);
    return false;
}

bool MongoDb::MainLoop(time_t tmTick)
{
    if ((get_curr_time() - _tmLast) >= 60)
    {
        _tmLast = get_curr_time();
        return _pMongo->DbPing();
    }
    return true;
}

bool MongoDb::DbInsertLogLst(DbRequest &request)
{
	LogName(request.tbl(), request.sectbl().name());
    stream::document qer, jIns;

	BEGIN_TRY_MD
	{
        MongoDbField::MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer);
        MongoDbField::MakeInsertLogLst(request, jIns);
		return _pMongo->_UpdateOne(request.tbl(), qer, jIns, true);
	}
	ENG_TRY_MD(qer, jIns);
	return false;
}

bool MongoDb::DbInsertLst(DbRequest &request, bool *pMatch)
{
	LogName(request.tbl(), request.sectbl().name());
    stream::document qer, jIns;

	BEGIN_TRY_MD
    {
        stream::document jne;
        MongoDbField::MakeInsertLst(request, jIns);
        if (!MongoDbField::MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer)
            || !MongoDbField::MakeKeyBson(request.reqMap(), request.S_KEY, "$ne", jne))
        {
            LOG_ERROR("_id is null!, %s-%s"
                , request.tbl().c_str(), request.sectbl().name().c_str());
            return false;
        }
        qer << request.sectbl().name() + "." + request.S_KEY << jne;

        bool bRet = false;
        bRet = _pMongo->_UpdateOne(request.tbl(), qer, jIns, true, pMatch);

        qer.clear();
        jIns.clear();
        return bRet;
    }
	ENG_TRY_MD(qer, jIns);
    return false;
}

bool MongoDb::DbDeleteLst(DBReqBase &request)
{
	LogName(request.tbl(), request.sectbl().name());
    stream::document qer, jDel;

	BEGIN_TRY_MD
    {
        if (!MongoDbField::MakeConDeleteLst(request, qer, jDel))
            return false;

        bool bDelMatch = false;
        bool bRet = _pMongo->_UpdateOne(request.tbl(), qer, jDel, false, &bDelMatch);
        if (!bDelMatch)
        {
            LOG_ERROR("delete row is empty! %s, %s", bsoncxx::to_json(qer).c_str()
                , bsoncxx::to_json(jDel).c_str());
        }
        return bRet;
    }
	ENG_TRY_MD(qer, jDel);
    return false;
}

bool MongoDb::DbUpdateLst(DbRequest &request, bool *pMatch)
{
	LogName(request.tbl(), request.sectbl().name());
    stream::document jbuild, qer;

	BEGIN_TRY_MD
    {
        if(!MongoDbField::MakeCon(request, qer)) return false;
        MongoDbField::MakeUpdateLst(request, jbuild);
        bool bRet = _pMongo->_UpdateOne(request.tbl(), qer, jbuild, false, pMatch);
        jbuild.clear();
        qer.clear();
        return bRet;
    }
	ENG_TRY_MD(qer, jbuild);
    return false;
}

bool MongoDb::DbInsertUpdateLst(DbRequest &request, bool bUpFirst)
{
	LogName(request.tbl(), request.sectbl().name());

    bool bMatch = false;

    if (bUpFirst)
    {
	    bool bRet = DbUpdateLst(request, &bMatch);
	    if (bRet)
	    {
            if(!bMatch)
		        return DbInsertLst(request);
            return bRet;
	    }
	    else
	    {
		    return DbInsertLst(request);
	    }
    }
    else
    {
        bool bRet = DbInsertLst(request, &bMatch);
        if (bRet)
        {
            if (!bMatch)
                return DbUpdateLst(request);
            return bRet;
        }
        else
        {
            return DbUpdateLst(request);
        }
    }
}


void MongoDb::DoError(const std::string& funcName, int lineno)
{
    //此处不能用BEGIN_TRY_MD，否则如果连接断开了，是死循环调用了
    _pMongo->CheckConnect();
    DbRequest stRequest(eDBOptType::INSERT_LOG, "u_error_log");
    stRequest.AddReqStr("exception", _strException);
    stRequest.AddReq("errorno", _errorno);
    stRequest.AddReqStr("query", _strQueryLog);
    stRequest.AddReqTime("time", get_curr_time());
    stRequest.AddReqStr("tablename", _tblName);
    stRequest.AddReqStr("sectablename", _secTblName);
    stRequest.AddReqStr("function", funcName);
    stRequest.AddReq("line", lineno);
    stRequest.AddReqStr("proc", file::get_exec_name());
    try
    {
        stream::document jbuild;
        MongoDbField::MakeBsonMap(stRequest.reqMap(), jbuild);
        _pMongo->_Insert(stRequest.tbl(), jbuild);
        jbuild.clear();
    }
    catch (std::exception& e)
    {
        LOG_ERROR(" %s,%s : %s"
            , stRequest.tbl().c_str()
            , stRequest.sectbl().name().c_str(), e.what());
        SetErr(e.what());
    }
}

void MongoDb::AddQueryLog(const std::string& query)
{
    _strQueryLog.append(query);
    _strQueryLog.append("; ");
}


const char* MongoDb::GetQueryLog()
{
    return _strQueryLog.c_str();
}

void MongoDb::ClearErr()
{
    _strException.clear();
    _errorno = 0;
}

void MongoDb::SetErr(const std::string& err, int32 errorno)
{
    _strException = err;
    _errorno = errorno;
}

void MongoDb::SetErr(const std::string& err, int32 errorno, const std::string& strLog)
{
    _errorno = errorno;
    _strException = err;
    _strQueryLog.append(strLog);
    _strQueryLog.append("; ");
}

void MongoDb::LogName(const std::string& tblName, const std::string& secTable /*= ""*/)
{
    _tblName = tblName;
    _secTblName = secTable;
    ClearErr();  
    _pMongo->CheckConnect();
}
