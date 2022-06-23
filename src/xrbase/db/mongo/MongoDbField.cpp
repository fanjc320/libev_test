/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "MongoDbField.h"
#include "db/base/DbDef.h"
#include "log/LogMgr.h"
#include "time/TimeDateX.h"
#include <math.h>
#include <chrono>
#include "mongocxx/exception/query_exception.hpp"
#include <bsoncxx/types.hpp>
#include "time/time_func_x.h"
#include "mongocxx/exception/bulk_write_exception.hpp"
#include "mongocxx/exception/query_exception.hpp"
#include "mongocxx/exception/write_exception.hpp"
#include "mongocxx/exception/error_code.hpp"
#include "mongocxx/exception/server_error_code.hpp"
#include <mongocxx/exception/logic_error.hpp>
#include "mongoc.h"
#include "MongoDb.h"

const int CalTypeNum = (int) eDBEQType::END;
std::string szCalType[CalTypeNum] = { "$eq", "$gt", "$lt", "$gte", "$lte", "$ne" };

void MongoDbField::ReadDBValue(const bsoncxx::document::element& objv, DBMap& mapRet)
{
    if (objv.raw() == nullptr) return;
    std::string strKey = objv.key().data();
    if (strKey.length() == 0) return;

    switch (objv.type())
    {
        case bsoncxx::type::k_int32:
            mapRet.AddInt(strKey.c_str(), objv.get_int32().value);
            break;
        case bsoncxx::type::k_int64:
            mapRet.AddInt(strKey.c_str(), objv.get_int64().value);
            break;
        case bsoncxx::type::k_date:
            mapRet.AddTime(strKey.c_str(), std::chrono::duration_cast<std::chrono::seconds>(objv.get_date().value));
            break;
        case bsoncxx::type::k_bool:
            mapRet.AddInt(strKey.c_str(), objv.get_bool().value);
            break;
        case bsoncxx::type::k_double:
            mapRet.AddDouble(strKey.c_str(), objv.get_double().value);
            break;
        case bsoncxx::type::k_utf8:
            {
                mapRet.AddStr(strKey.c_str(), std::string(objv.get_utf8().value));
            }
            break;
        case bsoncxx::type::k_oid:
            mapRet.AddStr(strKey.c_str(), objv.get_oid().value.to_string());
            break;
        case bsoncxx::type::k_binary:
            {
                auto bytes = objv.get_binary();
                string ret;
                ret.append((char*) bytes.bytes, bytes.size);
                mapRet.AddBinary(strKey.c_str(), ret);
            }
            break;
        case bsoncxx::type::k_array:
            {
                bsoncxx::types::b_array arr = objv.get_array();
                std::list<int64> lstID;
                std::list<std::string> lstStr;
                for (auto& itr : arr.value)
                {
                    if (itr.raw() == nullptr) continue;
                    if (itr.type() == bsoncxx::type::k_int32)
                        lstID.push_back(itr.get_int32());
                    else if (itr.type() == bsoncxx::type::k_int64)
                        lstID.push_back(itr.get_int64());
                    else if (itr.type() == bsoncxx::type::k_double)
                        lstID.push_back((int64) itr.get_double());
                    else if (itr.type() == bsoncxx::type::k_utf8)
                        lstStr.push_back(itr.get_utf8().value.data());
                }
                if (lstID.size() > 0)
                    mapRet.AddList(strKey.c_str(), lstID);
                if (lstStr.size() > 0)
                    mapRet.AddListStr(strKey.c_str(), lstStr);
            }
            break;
        default:
            break;
    }
}

bool MongoDbField::SelectOne(DBRet& ret, const bsoncxx::document::view& values)
{
    ret.clear();
    for (auto& objv : values)
    {
        if (objv.raw() == nullptr) continue;
        std::string szKey = objv.key().data();
        if (szKey.empty()) continue;
        ret.AddField(szKey);
        switch (objv.type())
        {
            case bsoncxx::type::k_array:
                {
                    bsoncxx::types::b_array vitem = objv.get_array();
                    bool bSimple = false;
                    if (vitem.value.begin() != vitem.value.end())
                    {
                        auto atype = vitem.value.begin()->type();
                        if (atype != bsoncxx::type::k_document)
                            bSimple = true;
                    }
                    if (bSimple)
                    {
                        std::list<int64> lstID;
                        std::list<std::string> lstStr;
                        for (auto& objdata : vitem.value)
                        {
                            if (objdata.raw() == nullptr) continue;
                            if (objdata.type() == bsoncxx::type::k_int32)
                                lstID.push_back(objdata.get_int32());
                            else if (objdata.type() == bsoncxx::type::k_int64)
                                lstID.push_back(objdata.get_int64());
                            else if (objdata.type() == bsoncxx::type::k_double)
                                lstID.push_back((int64) objdata.get_double());
                            else if (objdata.type() == bsoncxx::type::k_utf8)
                                lstStr.push_back(objdata.get_utf8().value.data());
                        }
                        if (lstID.size() > 0)
                            ret.Ret_Main().AddList(szKey.c_str(), lstID);
                        if (lstStr.size() > 0)
                            ret.Ret_Main().AddListStr(szKey.c_str(), lstStr);
                    }
                    else
                    {
                        sDBSecRet& vecValue = ret.Ret_Vec(szKey.c_str());
                        size_t vSize = std::distance(vitem.value.begin(), vitem.value.end());
                        vecValue.resize(vSize);
                        size_t nIndex = 0;
                        for (auto& objdata : vitem.value)
                        {
                            if (objdata.type() == bsoncxx::type::k_document)
                            {
                                bsoncxx::types::b_document vData = objdata.get_document();
                                for (auto& item : vData.view())
                                {
                                    //暂时字段没啥用
                                    //if (item.raw() == nullptr) continue;
                                    //std::string szItem = item.key().to_string();
                                    //if (szItem.length() == 0) continue;
                                    //
                                    //ret.AddField(szKey + "." + szItem);
                                    ReadDBValue(item, vecValue[nIndex]);
                                }
                                ++nIndex;
                            }
                        }
                    }
                }
                break;
            case bsoncxx::type::k_document:
                {
                    sDBSecRet& vecValue = ret.Ret_Vec(szKey.c_str());
                    vecValue.resize(vecValue.size() + 1);
                    DBMap& mapRet = vecValue[vecValue.size()-1];
                    bsoncxx::types::b_document doc = objv.get_document();
                    for (auto& item : doc.value)
                    {
                        //暂时字段没啥用
                        //if (item.raw() == nullptr) continue;
                        //std::string szItem = item.key().to_string();
                        //if (szItem.length() == 0) continue;
                        //ret.AddField(szKey + "." + szItem);
                        ReadDBValue(item, mapRet);
                    }
                }
                break;
            default:
                ReadDBValue(objv, ret.Ret_Main());
                break;
        }
    }
    return true;
}


void MongoDbField::MakeUpdate(DbRequest& request, stream::document& jbuild)
{
    stream::document jadd;
    MakeBsonMap(request.reqMap(), jadd);
    jbuild << "$set" << jadd;
    jadd.clear();
}

bool MongoDbField::MakeCon(DBReqBase& request, stream::document& qer)
{
    bool bExt = false;
    if (request.sectbl().IsList()) //二级表
    {
        std::string strIndex = request.sectbl().name() + "." + request.S_KEY;
        if (!MakeKeyBson(request.Con(), request.S_KEY, strIndex, qer))
        {
            LOG_ERROR("%s-%s: id is null", request.tbl().c_str()
                , request.sectbl().name().c_str());
            return false;
        }
        bExt = true;
    }

    MakeBson(request.Con().MapNum(), qer, "", bExt ? request.S_KEY : "");
    MakeBson(request.Con().MapStr(), qer, "", bExt ? request.S_KEY : "");
    return true;
}

bool MongoDbField::MakeConMany(DBReqBase& request, stream::document& qer)
{
    MakeBson(request.Con().MapNum(), qer);
    MakeBson(request.Con().MapStr(), qer);
    return true;
}

bool MongoDbField::MakeConDelete(DBReqBase& request, stream::document& qer, bool& bMul)
{
    size_t ret = MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer, true);
    if (ret == 0)
    {
        LOG_ERROR("_id is null!, will delete all!, %s-%s"
            , request.tbl().c_str(), request.sectbl().name().c_str());
        return false;
    }
    bMul = (ret > 1);

    if (!MakeCon(request, qer)) return false;

    return true;
}

bool MongoDbField::MakeConDeleteLst(DBReqBase& request, stream::document& qer, stream::document& jDel)
{
    stream::document jadd, jaddSub;

    if (!MakeKeyBson(request.Con(), request.S_KEY, request.S_KEY, jaddSub, true))
    {
        LOG_ERROR("id is null!, will delete all sectable!, %s-%s"
            , request.tbl().c_str(), request.sectbl().name().c_str());
        return false;
    }
    jadd << request.sectbl().name() << jaddSub;
    jDel << "$pull" << jadd;

    if (!MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer, true))
    {
        jadd.clear();
        jaddSub.clear();

        LOG_ERROR("_id is null!, will delete all!, %s-%s"
            , request.tbl().c_str(), request.sectbl().name().c_str());
        return false;
    }

    MakeBson(request.Con().MapNum(), qer);
    MakeBson(request.Con().MapStr(), qer);

    jadd.clear();
    jaddSub.clear();

    return true;
}

void MongoDbField::MakeUpdateObject(DbRequest& request, stream::document& jbuild)
{
    stream::document jsub;
    MakeBsonMap(request.reqMap(), jsub, request.sectbl().name() + ".");
    jbuild << "$set" << jsub;
    jsub.clear();
}

void MongoDbField::MakeUpdateLst(DbRequest& request, stream::document& jbuild)
{
    stream::document jsub;
    MakeBsonMap(request.reqMap(), jsub, request.sectbl().name() + ".$.");
    jbuild << "$set" << jsub;
    jsub.clear();
}

void MongoDbField::MakeInsertLst(DbRequest& request, stream::document& jbuild)
{
    stream::document jadd, jsub;
    MakeBsonMap(request.reqMap(), jsub);
    if (request.sectbl().limit() != 0)
    {
        stream::document jPush;
        stream::array arr;
        arr << jsub;
        jPush << "$each" << arr;
        jPush << "$slice" << request.sectbl().limit();
        jadd << request.sectbl().name() << jPush;
    }
    else
        jadd << request.sectbl().name() << jsub;

    jbuild << "$push" << jadd;

    jadd.clear();
    jsub.clear();
}

void MongoDbField::MakeInsertLogLst(DbRequest& request, stream::document& jbuild)
{
    stream::document jadd, jsub;
    MakeBsonMap(request.reqMap(), jsub);
    jadd << request.sectbl().name() << jsub;
    jbuild << "$push" << jadd;
    jadd.clear();
    jsub.clear();
}

void MongoDbField::MakeBsonFields(DBReqBase& req, stream::document& jbuild)
{
    for (const auto& itr : req.Fields())
    {
        jbuild << itr << true;
    }

    if (req.Fields().size() == 0 && req.sectbl().IsList())
    {
        jbuild << req.sectbl().name() + ".$" << true;
    }
}

void MongoDbField::MakeBsonMap(DBMap& con, stream::document& jbuild, const std::string& strIndex)
{
    MakeBson(con.MapNum(), jbuild, strIndex);
    MakeBson(con.MapStr(), jbuild, strIndex);
    MakeBson(con.MapList(), jbuild, strIndex);
    MakeBson(con.MapListStr(), jbuild, strIndex);
}

size_t MongoDbField::MakeKeyBson(DBMap& con, const std::string& key, const std::string& keyEx
    , stream::document& jbuild, bool bErase)
{
    if (!con.MapNum().empty())
    {
        auto itrnum = con.MapNum().find(key);
        if (itrnum != con.MapNum().end())
        {
            if (itrnum->second.type == eDBFieldType::INTEGER)
                AppendNum(jbuild, keyEx, itrnum->second.value, itrnum->second.caltype);
            else if (itrnum->second.type == eDBFieldType::TIME)
                AppendTime(jbuild, keyEx, itrnum->second.value, itrnum->second.caltype);

            if (bErase) con.MapNum().erase(itrnum);
            return 1;
        }
    }

    if (!con.MapStr().empty())
    {
        auto itrstr = con.MapStr().find(key);
        if (itrstr != con.MapStr().end())
        {
            if (itrstr->second.type == eDBFieldType::STRING)
                AppendStr(jbuild, keyEx, itrstr->second.value, itrstr->second.caltype);
            else if (itrstr->second.type == eDBFieldType::BINARY)
                AppendBinary(jbuild, keyEx, itrstr->second.value, itrstr->second.caltype);

            if (bErase) con.MapStr().erase(itrstr);
            return 1;
        }
    }
    if (!con.MapList().empty())
    {
        auto itrLst = con.MapList().find(key);
        if (itrLst != con.MapList().end())
        {
            if (itrLst->second.size() == 0)
                return 0;

            size_t ret = itrLst->second.size();
            MakeBsonCon(itrLst->second, jbuild, keyEx);

            if (bErase) con.MapList().erase(itrLst);
            return ret;
        }
    }
    return 0;
}

void MongoDbField::MakeBson(umap<std::string, DBMap::DBItemNum>& mapInt, stream::document& jbuild
    , const std::string& strIndex /*= ""*/, const string& exField /*= ""*/)
{
    if (mapInt.empty()) return;

    for (auto itr = mapInt.begin(); itr != mapInt.end(); ++itr)
    {
        if (exField.size() > 0 && exField == itr->first) continue;

        if (itr->second.type == eDBFieldType::INTEGER)
        {
            if (strIndex.empty())
                AppendNum(jbuild, itr->first, itr->second.value, itr->second.caltype);
            else
                AppendNum(jbuild, (strIndex + itr->first), itr->second.value, itr->second.caltype);
        }
        else if (itr->second.type == eDBFieldType::TIME)
        {
            if (strIndex.empty())
                AppendTime(jbuild, itr->first, itr->second.value, itr->second.caltype);
            else
                AppendTime(jbuild, strIndex + itr->first, itr->second.value, itr->second.caltype);
        }
        else if (itr->second.type == eDBFieldType::DOUBLE)
        {
            if (strIndex.empty())
                AppendDouble(jbuild, itr->first, itr->second.dbl, itr->second.caltype);
            else
                AppendDouble(jbuild, strIndex + itr->first, itr->second.dbl, itr->second.caltype);
        }
    }
}

void MongoDbField::MakeBson(umap<std::string, DBMap::DBItemStr>& mapStr, stream::document& jbuild
    , const std::string& strIndex /*= ""*/, const string& exField /*= ""*/)
{
    if (mapStr.empty()) return;
    bsoncxx::types::b_binary bytes;
    bytes.sub_type = bsoncxx::binary_sub_type::k_binary;

    for (auto itr = mapStr.begin(); itr != mapStr.end(); ++itr)
    {
        if (exField.size() > 0 && exField == itr->first) continue;

        if (itr->second.type == eDBFieldType::STRING)
        {
            if (strIndex.empty())
                AppendStr(jbuild, itr->first, itr->second.value, itr->second.caltype);
            else
                AppendStr(jbuild, (strIndex + itr->first), itr->second.value, itr->second.caltype);
        }
        else if (itr->second.type == eDBFieldType::BINARY)
        {
            bytes.bytes = (uint8*) itr->second.value.c_str();
            bytes.size = (uint32_t) itr->second.value.size();
            if (strIndex.empty())
                AppendBinary(jbuild, itr->first, bytes, itr->second.caltype);
            else
                AppendBinary(jbuild, (strIndex + itr->first), bytes, itr->second.caltype);
        }
    }
}

void MongoDbField::AppendNum(stream::document& jbuild, const std::string& fieldName
    , const int64& value, eDBEQType eCalType)
{
    if (eCalType == eDBEQType::EQ)
    {
        if (value >= INT32_MAX || value <= INT32_MIN)
            jbuild << fieldName << (std::int64_t) value;
        else
            jbuild << fieldName << (std::int32_t) (value);
    }
    else
    {
        stream::document dSort;
        if (value >= INT32_MAX || value <= INT32_MIN)
            dSort << szCalType[(int) eCalType] << (std::int64_t) value;
        else
            dSort << szCalType[(int) eCalType] << (std::int32_t) (value);

        jbuild << fieldName << dSort;
    }
}

void MongoDbField::AppendTime(stream::document& jbuild, const std::string& fieldName
    , const int64& value, eDBEQType eCalType)
{
    if (eCalType == eDBEQType::EQ)
    {
        jbuild << fieldName << bsoncxx::types::b_date(std::chrono::milliseconds(value * 1000));
    }
    else
    {
        stream::document dSort;
        dSort << szCalType[(int) eCalType] << bsoncxx::types::b_date(std::chrono::milliseconds(value * 1000));
        jbuild << fieldName << dSort;
    }
}

void MongoDbField::AppendDouble(stream::document& jbuild, const std::string& fieldName, const double& value, eDBEQType eCalType)
{
    if (eCalType == eDBEQType::EQ)
    {
            jbuild << fieldName << value;
    }
    else
    {
        stream::document dSort;
        dSort << szCalType[(int) eCalType] << value;
        jbuild << fieldName << dSort;
    }

    jbuild << fieldName << value;
}

void MongoDbField::AppendStr(stream::document& jbuild, const std::string& fieldName
    , const std::string& value, eDBEQType eCalType)
{
    if (eCalType == eDBEQType::EQ)
    {
        jbuild << fieldName << value;
    }
    else
    {
        stream::document dSort;
        dSort << szCalType[(int) eCalType] << value;
        jbuild << fieldName << dSort;
    }
}

void MongoDbField::AppendBinary(stream::document& jbuild, const std::string& fieldName
    , const std::string& value, eDBEQType eCalType)
{
    bsoncxx::types::b_binary bytes;
    bytes.sub_type = bsoncxx::binary_sub_type::k_binary;
    bytes.bytes = (uint8*) value.c_str();
    bytes.size = (uint32_t) value.size();
    AppendBinary(jbuild, fieldName, bytes, eCalType);
}

void MongoDbField::AppendBinary(stream::document& jbuild, const std::string& fieldName
    , const bsoncxx::types::b_binary& value, eDBEQType eCalType)
{
    if (eCalType == eDBEQType::EQ)
    {
        jbuild << fieldName << value;

    }
    else
    {
        stream::document dSort;
        dSort << szCalType[(int) eCalType] << value;
        jbuild << fieldName << dSort;
    }
}

void MongoDbField::MakeArray(const std::list<int64>& lst, bsoncxx::builder::stream::array& arr)
{
    if (lst.empty()) return;

    for (auto& id : lst)
    {
        if (id >= INT32_MAX || id <= INT32_MIN)
            arr << (std::int64_t) id;
        else
            arr << (std::int32_t) (id);
    }
}

void MongoDbField::MakeArray(const std::list <std::string>& lst, bsoncxx::builder::stream::array& arr)
{
    if (lst.empty()) return;

    for (auto& id : lst)
    {
        arr << id;
    }
}

int64 MongoDbField::ReadInt(bsoncxx::document::view& vew, const char* szfield)
{
    if (vew.empty()) return 0;

    auto citr = vew.find(szfield);
    if (citr == vew.end()) return 0;
    const bsoncxx::document::element& obj = *citr;
    switch (obj.type())
    {
        case bsoncxx::type::k_int32:
            return (int64) obj.get_int32();
            break;
        case bsoncxx::type::k_int64:
            return obj.get_int64();
            break;
        case bsoncxx::type::k_double:
            return (int64) obj.get_double();
            break;
        case bsoncxx::type::k_bool:
            return (int64) obj.get_bool();
            break;
        default:
            break;
    }
    return 0;
}

std::string MongoDbField::ReadStr(bsoncxx::document::view& vew, const char* szfield)
{
    if (vew.empty()) return "";

    auto citr = vew.find(szfield);
    if (citr == vew.end()) return "";
    const bsoncxx::document::element& obj = *citr;
    switch (obj.type())
    {
        case bsoncxx::type::k_utf8:
            return obj.get_utf8().value.data();
            break;
        default:
            break;
    }
    return "";
}

std::string MongoDbField::ReadBinary(bsoncxx::document::view& vew, const char* szfield)
{
    if (vew.empty()) return "";

    auto citr = vew.find(szfield);
    if (citr == vew.end()) return "";
    const bsoncxx::document::element& obj = *citr;
    switch (obj.type())
    {
        case bsoncxx::type::k_binary:
            {
                auto bytes = obj.get_binary();
                string ret;
                ret.append((char*) bytes.bytes, bytes.size);
                return ret;
            }
            break;
        default:
            break;
    }
    return "";
}

std::string MongoDbField::ReadOID(bsoncxx::document::view& vew, const char* szfield)
{
    if (vew.empty()) return "";

    auto citr = vew.find(szfield);
    if (citr == vew.end()) return "";
    const bsoncxx::document::element& obj = *citr;
    switch (obj.type())
    {
        case bsoncxx::type::k_oid:
            return obj.get_oid().value.to_string();
            break;
        default:
            break;
    }
    return "";
}

double MongoDbField::ReadDouble(bsoncxx::document::view& vew, const char* szfield)
{
    if (vew.empty()) return 0.0;

    auto citr = vew.find(szfield);
    if (citr == vew.end()) return 0.0;
    const bsoncxx::document::element& obj = *citr;
    switch (obj.type())
    {
        case bsoncxx::type::k_int32:
            return (double) obj.get_int32();
            break;
        case bsoncxx::type::k_int64:
            return (double)obj.get_int64();
            break;
        case bsoncxx::type::k_double:
            return obj.get_double();
            break;
        case bsoncxx::type::k_bool:
            return (double) obj.get_bool();
            break;
        default:
            break;
    }
    return 0.0;
}

bool MongoDbField::IsReconCode(int err)
{
    //int nerr = err.code().value();
    switch (err)
    {
        case MONGOC_ERROR_STREAM_SOCKET:
        case MONGOC_ERROR_STREAM_CONNECT:
        case MONGOC_ERROR_SERVER_SELECTION_FAILURE:
        case MONGOC_ERROR_STREAM_INVALID_TYPE:
        case MONGOC_ERROR_STREAM_INVALID_STATE:
        case MONGOC_ERROR_STREAM_NAME_RESOLUTION:
        case MONGOC_ERROR_STREAM_NOT_ESTABLISHED:
            return true;
            break;
        default:
            return false;
    }
    return false;
}

void MongoDbField::SetErr(MongoDb* pDb, const mongocxx::operation_exception* err)
{
    int _errorno = err->code().value();
    std::string strLog;
    if (err->raw_server_error())
    {
        strLog = bsoncxx::to_json(*(err->raw_server_error()));
    }
    std::string strError = err->what();
    pDb->SetErr(strError, _errorno, strLog);
}
