/*
 *
 *      Author: venture
 */
#pragma once
#include <list>
#include <functional>

#include "dtype.h"
#include "mongocxx/client.hpp"
#include "mongocxx/instance.hpp"
#include "bsoncxx/stdx/optional.hpp"
#include "platform/file_func.h"
#include "bsoncxx/builder/stream/document.hpp"
#include "bsoncxx/json.hpp"
#include "bsoncxx/builder/stream/array.hpp"
#include "mongocxx/exception/operation_exception.hpp"
#include "db/base/DbDef.h"

using namespace bsoncxx::builder;
typedef bsoncxx::stdx::optional<bsoncxx::document::value> optx;

class MongoDbBase
{
public:
    MongoDbBase();
    virtual ~MongoDbBase();

    //auth:MONGODB-CR, SCRAM-SHA-1
    virtual bool Init(const std::string& hostname, const std::string& user, 
        const std::string& pwd, const std::string& dbname, const std::string& auth, int32_t Areano);

    void UnInit();

	bool CheckConnect();

	bool DbPing();

    static std::string MakeOID();
    virtual void DoError(const std::string& funcName, int lineno) { };
    mongocxx::collection* GetCollection(const std::string& colname);
    bool HasCollection(const std::string& colname);

public:
    virtual bool _DropCollection(const std::string& ns);

    virtual bool _UpdateOne(const std::string& ns, stream::document& query
        , stream::document& obj, bool upsert = false, bool* bMatch = nullptr);

    virtual bool _UpdateMany(const std::string& ns, stream::document& query
        , stream::document& obj, bool upsert = false);

    virtual optx _FindOne(const std::string& ns, stream::document& query,
        stream::document* fieldsToReturn = nullptr);

    virtual bool _IsExsit(const std::string& ns, stream::document& query
        , stream::document* fieldsToReturn = nullptr);

    virtual bool _Insert(const std::string& ns, stream::document& query);
    virtual bool _Insert(const std::string& ns, const bsoncxx::document::view & query);

    mongocxx::cursor _FindN(const std::string& ns,
        stream::document& query, int nToReturn, int nToSkip = 0
        , stream::document* fieldsToReturn = nullptr, stream::document* pSortdoc = nullptr);

    virtual std::int64_t _Count(const std::string& ns, stream::document& query);
    virtual bool _Remove(const std::string& ns, stream::document& query, bool justOne, bool bEmptyErr);
    optx _FindAndModify(const std::string& ns, stream::document& query,
        stream::document& update, bool upsert = false, stream::document* fieldsToReturn = nullptr);

protected:
    static mongocxx::instance  instx;
    std::string         _szurl;
    mongocxx::client    _connx;
    mongocxx::database  _dbx;
    mongocxx::uri       _urlx;
    umap<std::string, mongocxx::collection> _mapCols;

    std::string		_hostname;
	std::string		_user;
	std::string		_pwd;
	std::string		_dbname;
    std::string     _auth;
	bool			_bInit = false;
};


