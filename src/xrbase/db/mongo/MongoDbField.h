/*
 *
 *      Author: venture
 */
#pragma once

#include "mongocxx/client.hpp"
#include "mongocxx/instance.hpp"
#include "bsoncxx/stdx/optional.hpp"
#include "platform/file_func.h"
#include "bsoncxx/builder/stream/document.hpp"
#include "bsoncxx/json.hpp"
#include "bsoncxx/builder/stream/array.hpp"
#include "mongocxx/exception/operation_exception.hpp"
#include "db/base/DbDef.h"
#include "MongoDef.h"

class MongoDb;
using namespace bsoncxx::builder;
class MongoDbField
{
public:
    static bool SelectOne(DBRet& ret, const bsoncxx::document::view& values);
    static void ReadDBValue(const bsoncxx::document::element& objv, DBMap& mapRet);

    static void MakeUpdate(DbRequest& request, stream::document& jbuild);
    static bool MakeCon(DBReqBase& request, stream::document& qer);
    static bool MakeConMany(DBReqBase& request, stream::document& qer);

    static bool MakeConDelete(DBReqBase& request, stream::document& qer, bool& bMul);
    static bool MakeConDeleteLst(DBReqBase& request, stream::document& qer, stream::document& jDel);

    static void MakeUpdateObject(DbRequest& request, stream::document& jbuild);

    static void MakeUpdateLst(DbRequest& request, stream::document& jbuild);
    static void MakeInsertLst(DbRequest& request, stream::document& jbuild);
    static void MakeInsertLogLst(DbRequest& request, stream::document& jbuild);

    static void MakeBsonFields(DBReqBase& req, stream::document& jbuild);
    static void MakeBsonMap(DBMap& con, stream::document& jbuild, const std::string& strIndex = "");

    //用于一级，二级KEY的生成
    static size_t MakeKeyBson(DBMap& con, const std::string& key, const std::string& keyEx
        , stream::document& jbuild, bool bErase = false);

    static void MakeBson(umap<std::string, DBMap::DBItemStr>& mapStr, stream::document& jbuild
        , const std::string& strIndex = "", const string& exField = "");
    static void MakeBson(umap<std::string, DBMap::DBItemNum>& mapInt, stream::document& jbuild
        , const std::string& strIndex = "", const string& exField = "");

    template <typename K_TYPE>
    static void MakeBson(umap<std::string, std::list<K_TYPE>>& mapArray, stream::document& jbuild
        , const std::string& strIndex = "");
    template <typename K_TYPE>
    static void MakeBsonCon(umap<std::string, std::list<K_TYPE>>& mapArray, stream::document& jbuild
        , const std::string& strIndex = "");
    template <typename K_TYPE>
    static void MakeBsonCon(std::list<K_TYPE>& mapArray, stream::document& jbuild, const std::string& strKey);

    static void AppendNum(stream::document& jbuild, const std::string& fieldName, const int64& value, eDBEQType eCalType);
    static void AppendTime(stream::document& jbuild, const std::string& fieldName, const int64& value, eDBEQType eCalType);
    static void AppendStr(stream::document& jbuild, const std::string& fieldName, const std::string& value, eDBEQType eCalType);
    static void AppendDouble(stream::document& jbuild, const std::string& fieldName, const double& value, eDBEQType eCalType);

    static void AppendBinary(stream::document& jbuild, const std::string& fieldName, const std::string& value, eDBEQType eCalType);
    static void AppendBinary(stream::document& jbuild, const std::string& fieldName, const bsoncxx::types::b_binary& value, eDBEQType eCalType);

    static void MakeArray(const std::list<int64>& lst, bsoncxx::builder::stream::array& arr);
    static void MakeArray(const std::list<std::string>& lst, bsoncxx::builder::stream::array& arr);

    static int64 ReadInt(bsoncxx::document::view& vew, const char* szfield);
    static std::string ReadStr(bsoncxx::document::view& vew, const char* szfield);
    static std::string ReadBinary(bsoncxx::document::view& vew, const char* szfield);
    static std::string ReadOID(bsoncxx::document::view& vew, const char* szfield);
    static double ReadDouble(bsoncxx::document::view& vew, const char* szfield);

    static bool IsReconCode(int err);
    static void SetErr(MongoDb* pDb, const mongocxx::operation_exception* err);
};

template <typename K_TYPE>
inline void MongoDbField::MakeBsonCon(umap<std::string, std::list<K_TYPE>>& mapArray, stream::document& jbuild, const std::string& strIndex)
{
    stream::document jin;
    bsoncxx::builder::stream::array arr;
    for (auto itr = mapArray.begin(); itr != mapArray.end(); ++itr)
    {
        if (itr->second.size() == 0) continue;
        jin.clear();
        arr.clear();
        MakeArray(itr->second, arr);
        jin << "$in" << arr;
        if (strIndex.empty())
            jbuild << itr->first << jin;
        else
            jbuild << (strIndex + itr->first) << jin;
    }
}

template <typename K_TYPE>
void MongoDbField::MakeBsonCon(std::list<K_TYPE>& mapArray, stream::document& jbuild, const std::string& strKey)
{
    stream::document jin;
    bsoncxx::builder::stream::array arr;

    MakeArray(mapArray, arr);
    jin << "$in" << arr;
    jbuild << strKey << jin;
}

template <typename K_TYPE>
inline void MongoDbField::MakeBson(umap<std::string, std::list<K_TYPE>>& mapArray, stream::document& jbuild, const std::string& strIndex)
{
    bsoncxx::builder::stream::array arr;
    for (auto itr = mapArray.begin(); itr != mapArray.end(); ++itr)
    {
        if (itr->second.size() == 0) continue;
        arr.clear();
        MakeArray(itr->second, arr);
        if (strIndex.empty())
            jbuild << itr->first << arr;
        else
            jbuild << (strIndex + itr->first) << arr;
    }
}
