/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "MongoDbBase.h"
#include <exception>
#include "mongocxx/exception/bulk_write_exception.hpp"
#include "mongocxx/exception/query_exception.hpp"
#include "mongocxx/exception/write_exception.hpp"
#include "mongocxx/exception/error_code.hpp"
#include "mongocxx/exception/server_error_code.hpp"
#include <mongocxx/exception/logic_error.hpp>
#include "string/str_format.h"
#include "log/LogMgr.h"
#include <mongoc.h>
#include <thread>
#include "dtype.h"
#include <chrono>
#include "db/mongo/MongoDef.h"
#include "MongoDbField.h"

const int32_t MAX_WAIT_MLLS = 60000;
mongocxx::instance  MongoDbBase::instx{};
MongoDbBase::MongoDbBase()
{
}

MongoDbBase::~MongoDbBase()
{
    
}

void MongoDbBase::UnInit()
{

}

bool MongoDbBase::Init(const std::string& hostname, const std::string& user, const std::string& pwd, 
    const std::string& dbname, const std::string& auth, int32_t Areano)
{
    _hostname = hostname;
	_user = user;
	_pwd = pwd;
	_dbname = dbname;
    _auth = auth;

    if (Areano > 0)
    {
        _dbname += "_";
        _dbname += std::to_string(Areano);
    }

    char szurl[1024] = {};

    if (_auth.empty())
    {
        sprintf_safe(szurl, "mongodb://%s:%s@%s/admin?serverSelectionTryOnce=false"
            , user.c_str(), pwd.c_str(), hostname.c_str());
    }
    else
    {
        sprintf_safe(szurl, "mongodb://%s:%s@%s/admin?authMechanism=%s&serverSelectionTryOnce=false"
            , user.c_str(), pwd.c_str(), hostname.c_str(), _auth.c_str());
    }

    _szurl.clear();
    _szurl.append(szurl, sizeof(szurl));

    return CheckConnect();
}

bool MongoDbBase::CheckConnect()
{
    if (_bInit) return true;
    try
    {    
        _mapCols.clear();
        
        _urlx = mongocxx::uri{ _szurl };
        _connx = mongocxx::client{_urlx};
        _dbx = _connx.database(_dbname);

        stream::document docx;
        docx << "getLastError" << 1;
        _dbx.run_command(docx.view());

        _bInit = true;
    }
    catch (const mongocxx::operation_exception& err)
    {
        int nerr = err.code().value();
        LOG_ERROR("%s, code:%d", err.what(), nerr);
        _bInit = false;
        return false;
    }
    catch (std::exception& err)
    {
        LOG_ERROR("%s", err.what());
        _bInit = false;
        return false;
    }
    return _bInit;
}

bool MongoDbBase::DbPing()
{
    if (!_bInit)
    {
        if(!CheckConnect()) return false;
        return true;
    }

    try
    {
        stream::document docx;
        docx << "getLastError" << 1;
        _dbx.run_command(docx.view());
        return true;
    }
    catch (const mongocxx::operation_exception& err)
    {
        _bInit = false;
        _mapCols.clear();
        int nerr = err.code().value();
        LOG_ERROR("%s, code:%d", err.what(), nerr);
        return false;
    }
    catch (std::exception& err)
    {
        _bInit = false;
        _mapCols.clear();
        LOG_ERROR("%s", err.what());
        return false;
    }

	return true;
}

mongocxx::collection* MongoDbBase::GetCollection(const std::string& colname)
{
    if (!CheckConnect())
        return nullptr;

    auto itr = _mapCols.find(colname);
    if (itr != _mapCols.end())
        return &(itr->second);
    else
    {
        _mapCols[colname] = _dbx.collection(colname);
        return &_mapCols[colname];
    }
}

bool MongoDbBase::HasCollection(const std::string& colname)
{
    if (!CheckConnect())
        return false;

    FORDO_TRY_BEGIN
    return _dbx.has_collection(colname);
    FORDO_TRY_END(mongocxx::v_noabi::operation_exception);
    return false;
}

std::string MongoDbBase::MakeOID()
{
    return bsoncxx::oid().to_string();
}

bool MongoDbBase::_DropCollection(const std::string& ns)
{
    FORDO_TRY_BEGIN
    {
        if (!_dbx.has_collection(ns)) return true;

        mongocxx::collection* col = GetCollection(ns);
        if (col == nullptr) throw std::runtime_error("colletion error!!");

        col->drop();
        _mapCols.erase(ns);
        return true;
    }
    FORDO_TRY_END(mongocxx::v_noabi::operation_exception);
    return false;
}

bool MongoDbBase::_UpdateOne(const std::string& ns, stream::document& query
    , stream::document& obj, bool upsert /*= false*/, bool* pMatch)
{
     mongocxx::options::update up{};
     up.upsert(upsert);
     FORDO_TRY_BEGIN
     {
         mongocxx::collection* col = GetCollection(ns);
        if (col == nullptr) throw std::runtime_error("colletion error!!");

        auto ret = col->update_one(query.view(), obj.view(), up);
        if (pMatch != nullptr)
        {
            if (ret == mongocxx::stdx::nullopt)
                *pMatch = false;
            else if ((ret->result().inserted_count() == 0
                    && ret->result().modified_count() == 0
                    && ret->result().upserted_count() == 0
                    && ret->result().matched_count() == 0
                    && ret->result().deleted_count() == 0))
            {
                *pMatch = false;
            }
            else
                *pMatch = true;
        }
        else
        {
            if (ret == mongocxx::stdx::nullopt ||
                (ret->result().inserted_count() == 0
                    && ret->result().modified_count() == 0
                    && ret->result().upserted_count() == 0
                    && ret->result().matched_count() == 0
                    && ret->result().deleted_count() == 0))
            {
                DoError(__FUNCTION__, __LINE__);
                LOG_ERROR("UpdateOne error:%s-%s-%s", ns.c_str()
                    , bsoncxx::to_json(query).c_str(), bsoncxx::to_json(obj).c_str());
            }
        }
        ret.reset();
        return true;
     }
     FORDO_TRY_END(mongocxx::v_noabi::bulk_write_exception);
     return false;
}

bool MongoDbBase::_UpdateMany(const std::string& ns, stream::document& query
    , stream::document& obj, bool upsert /*= false*/)
{
    mongocxx::options::update up{};
    up.upsert(upsert);
    FORDO_TRY_BEGIN
    {
        mongocxx::collection* col = GetCollection(ns);
        if (col == nullptr) throw std::runtime_error("colletion error!!");

        auto ret = col->update_many(query.view(), obj.view(), up);
        if (ret == mongocxx::stdx::nullopt)
        {
            DoError(__FUNCTION__, __LINE__);
            LOG_ERROR("insert error:%s-%s-%s", ns.c_str()
                , bsoncxx::to_json(query).c_str(), bsoncxx::to_json(obj).c_str());
        }
        ret.reset();
        return true;
    }
    FORDO_TRY_END(mongocxx::v_noabi::bulk_write_exception);
    return false;
}

optx MongoDbBase::_FindOne(const std::string& ns, stream::document& query
    , stream::document* fieldsToReturn)
{
    if (fieldsToReturn != nullptr)
    {
        mongocxx::options::find fd{};
        fd.projection(fieldsToReturn->view());
        //fd.max_time(std::chrono::milliseconds(MAX_WAIT_MLLS));
        //fd.max_await_time(std::chrono::milliseconds(MAX_WAIT_MLLS));

        FORDO_TRY_BEGIN
        {
            mongocxx::collection* col = GetCollection(ns);
            if (col == nullptr) throw std::runtime_error("colletion error!!");

            return col->find_one(query.view(), fd);
        }
        FORDO_TRY_END(mongocxx::v_noabi::query_exception);
    }
    else
    {
        FORDO_TRY_BEGIN
        {
            mongocxx::collection* col = GetCollection(ns);
            if (col == nullptr) throw std::runtime_error("colletion error!!");

            return col->find_one(query.view());
        }
        FORDO_TRY_END(mongocxx::v_noabi::query_exception);
    }
    return mongocxx::stdx::nullopt;
}

bool MongoDbBase::_IsExsit(const std::string& ns, stream::document& query 
    , stream::document* fieldsToReturn)
{
    if (fieldsToReturn != nullptr)
    {
        mongocxx::options::find fd{};
        fd.projection(fieldsToReturn->view());
        //fd.max_time(std::chrono::milliseconds(MAX_WAIT_MLLS));
        //fd.max_await_time(std::chrono::milliseconds(MAX_WAIT_MLLS));

        FORDO_TRY_BEGIN
        {
            mongocxx::collection* col = GetCollection(ns);
            if (col == nullptr) throw std::runtime_error("colletion error!!");

            return col->find_one(query.view(), fd) != mongocxx::stdx::nullopt;
        }
        FORDO_TRY_END(mongocxx::v_noabi::query_exception);
    }
    else
    {
        FORDO_TRY_BEGIN
        {
            mongocxx::collection* col = GetCollection(ns);
            if (col == nullptr) throw std::runtime_error("colletion error!!");

            return col->find_one(query.view()) != mongocxx::stdx::nullopt;
        }
        FORDO_TRY_END(mongocxx::v_noabi::query_exception);
    }
    return false;
}

bool MongoDbBase::_Insert(const std::string& ns, stream::document& query)
{
    FORDO_TRY_BEGIN
    {
        mongocxx::collection* col = GetCollection(ns);
        if (col == nullptr) throw std::runtime_error("colletion error!!");

        auto ret = col->insert_one(query.view());
        if (ret == mongocxx::stdx::nullopt || 
            (ret->result().inserted_count() == 0
                && ret->result().modified_count() == 0
                && ret->result().upserted_count() == 0))
        {
            DoError(__FUNCTION__, __LINE__);
            LOG_ERROR("insert error:%s-%s", ns.c_str(), bsoncxx::to_json(query).c_str());
        }
        ret.reset();
        return true;
    }
    FORDO_TRY_END(mongocxx::v_noabi::bulk_write_exception);
    return false;
}


bool MongoDbBase::_Insert(const std::string& ns, const bsoncxx::document::view & query)
{
    FORDO_TRY_BEGIN
    {
        mongocxx::collection* col = GetCollection(ns);
        if (col == nullptr) throw std::runtime_error("colletion error!!");

        auto ret = col->insert_one(query);
        if (ret == mongocxx::stdx::nullopt ||
        (ret->result().inserted_count() == 0
            && ret->result().modified_count() == 0
            && ret->result().upserted_count() == 0))
        {
            DoError(__FUNCTION__, __LINE__);
            LOG_ERROR("insert error:%s-%s", ns.c_str(), bsoncxx::to_json(query).c_str());
        }
        ret.reset();
        return true;
    }
    FORDO_TRY_END(mongocxx::v_noabi::bulk_write_exception);
    return false;
}

mongocxx::cursor MongoDbBase::_FindN(const std::string& ns
    , stream::document& query, int nToReturn, int nToSkip
    , stream::document* fieldsToReturn, stream::document* pSortdoc
    )
{
    mongocxx::options::find fd{};
    if (fieldsToReturn != nullptr)
    {
        fd.projection(fieldsToReturn->view());
    }
    if(nToSkip != 0) fd.skip(nToSkip);
    if(nToReturn != 0) fd.limit(nToReturn);
    if (pSortdoc != nullptr) fd.sort(pSortdoc->view());

    mongocxx::collection* col = GetCollection(ns);
    if (col == nullptr) throw std::runtime_error("colletion error!!");

    return col->find(query.view(), fd);
}

std::int64_t MongoDbBase::_Count(const std::string& ns, stream::document& query /*= Query()*/)
{
    FORDO_TRY_BEGIN
    {
        mongocxx::collection* col = GetCollection(ns);
        if (col == nullptr) throw std::runtime_error("colletion error!!");

        return col->count_documents(query.view());
    }
    FORDO_TRY_END(mongocxx::v_noabi::query_exception);
    return 0;
}

bool MongoDbBase::_Remove(const std::string& ns, stream::document& query, bool justOne /*= 0*/, bool bEmptyErr)
{
    if (justOne)
    {
        FORDO_TRY_BEGIN
        {
            mongocxx::collection* col = GetCollection(ns);
            if (col == nullptr) throw std::runtime_error("colletion error!!");

           auto ret = col->delete_one(query.view());
           if (bEmptyErr && (ret == mongocxx::stdx::nullopt || ret->deleted_count() == 0))
           {
               LOG_ERROR("delete error:%s-%s", ns.c_str(), bsoncxx::to_json(query).c_str());
               DoError(__FUNCTION__, __LINE__);
           }
           return true;
        }
        FORDO_TRY_END(mongocxx::v_noabi::bulk_write_exception);
    }
    else
    {
        FORDO_TRY_BEGIN
        {
            mongocxx::collection* col = GetCollection(ns);
            if (col == nullptr) throw std::runtime_error("colletion error!!");

            auto ret = col->delete_many(query.view());
            if (bEmptyErr && (ret == mongocxx::stdx::nullopt || ret->deleted_count() == 0))
            {
                LOG_ERROR("delete error:%s-%s", ns.c_str(), bsoncxx::to_json(query).c_str());
                DoError(__FUNCTION__, __LINE__);
            }
            return true;
        }
        FORDO_TRY_END(mongocxx::v_noabi::bulk_write_exception);
    }
    return false;
}

optx MongoDbBase::_FindAndModify(const std::string& ns, stream::document& query
    , stream::document& update, bool upsert /*= false*/, stream::document* fieldsToReturn)
{
    mongocxx::options::find_one_and_update up;
    up.upsert(upsert);
    if(fieldsToReturn != nullptr)
        up.projection(fieldsToReturn->view());

    up.return_document(mongocxx::options::return_document::k_after);
    FORDO_TRY_BEGIN
    {
        mongocxx::collection* col = GetCollection(ns);
        if (col == nullptr) throw std::runtime_error("colletion error!!");

        return col->find_one_and_update(query.view(), update.view(), up);
    }
    FORDO_TRY_END(mongocxx::v_noabi::write_exception);
    return mongocxx::stdx::nullopt;
}
