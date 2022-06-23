/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <typeinfo>

#include "dtype.h"
#include "MysqlDb.h"
#include "platform/file_func.h"
#include "log/LogMgr.h"
#include "string/str_num.h"
#include "string/str_split.h"
#include "mysql++/mysql++.h"
#include "mysql++/dbdriver.h"

bool _IsStrType(const mysqlpp::FieldTypes::value_type& stype)
{
    if (stype == typeid(std::string))
        return true;
    else if (stype == typeid(mysqlpp::String))
        return true;
    else if (stype == typeid(mysqlpp::sql_date))
        return true;
    else if (stype == typeid(mysqlpp::sql_time))
        return true;
    else if (stype == typeid(mysqlpp::sql_datetime))
        return true;
    else if (stype == typeid(mysqlpp::sql_set))
        return true;

    return false;
}

size_t _FetchRows(mysqlpp::StoreQueryResult& res, DBMap& ret)
{
    string fieldName, fieldValue;
    ret.clear();
    std::vector<bool> vType;
    vType.resize(res.num_fields(), false);
    for (size_t i = 0; i < res.num_rows(); ++i)
    {
        for (int r = 0; r < res.num_fields(); ++r)
        {
            if (i == 0)
            {
                if (_IsStrType(res.field_type(r)))
                    vType[r] = true;
            }

            fieldName = string(res.field_name(r));
            auto length = res[i][r].length();
            fieldValue = string(res[i][r].c_str(), length);
            if (vType[r])
                ret.AddStr(fieldName, fieldValue);
            else
                ret.AddInt(fieldName, stoll_x(fieldValue));
        }
    }
    return res.num_rows();
}

MysqlDb::MysqlDb()
{
    if (_pMysql == nullptr)
        _pMysql = new mysqlpp::Connection();
    _strHost = "localhost";
    _strUser = "root";
    _strPwd = "";
    _dwPort = 3306;
    _strName = "test";
}

MysqlDb::MysqlDb(const string& host, const string& user
    , const string& password, const string& name, int& port)
{
    if (_pMysql == nullptr)
        _pMysql = new mysqlpp::Connection();
    _strHost = host;
    _strUser = user;
    _strPwd = password;
    _dwPort = port;
    _strName = name;
}

MysqlDb::~MysqlDb()
{
    SAFE_DELETE(_pMysql);
}

bool MysqlDb::Init(const string& host, const string& user
    , const string& password
    , const string& name, const int& port)
{
    _strHost = host;
    _strUser = user;
    _strPwd = password;
    _dwPort = port;
    _strName = name;
    _szQuery.clear();

    return DbConnect();
}

void MysqlDb::UnInit()
{

}

bool MysqlDb::DbConnect()
{
    time_t tmBegin = std::time(nullptr);
    try
    {
        _pMysql->disable_exceptions();
        _pMysql->disconnect();
        _pMysql->set_option(new mysqlpp::SetCharsetNameOption("utf8"));
        _pMysql->set_option(new mysqlpp::ConnectTimeoutOption(30));//s

        if (_pMysql->connect(_strName.c_str(), _strHost.c_str()
            , _strUser.c_str(), _strPwd.c_str(), _dwPort))
        {
            _pMysql->query("SET NAMES utf8");
            LOG_INFO("DB connection success!, second:%lld", std::time(nullptr) - tmBegin);
            return true;
        }
        _pMysql->enable_exceptions();
        _szQuery.reserve(1024);
    }
    catch (mysqlpp::Exception& e)
    {
        LOG_ERROR("failed! error:%s, err:%s, second:%lld"
            , _GetDBLastError(), e.what(), std::time(nullptr) - tmBegin);
    }

    LOG_ERROR("DB connection failed:%s, second:%lld"
        , _GetDBLastError(), std::time(nullptr) - tmBegin);
    return false;
}

void MysqlDb::DbDisConnect()
{
    return _pMysql->disconnect();
}

bool MysqlDb::IsConnected()
{
    return _pMysql->connected();
}

bool MysqlDb::DBSelect_One(DbSelectReq& request)
{
    request.clear_ret();
    try
    {
        std::string strFields = _MakeFields(request.Fields());
        std::string strCon = _MakeCon(request.Con());
        _szQuery =  "SELECT " + strFields + " FROM `" + request.tbl() + "` WHERE " + strCon + ";";
        FUNC_DB_MAP_RET fnCall = [&](DBMap& ret)->bool { return true; };
        size_t nRetNum = 0;
        return _DBSelect(_szQuery, fnCall, request.Ret_Main(), nRetNum, true);
    }
    catch (const std::exception& err)
    {
        LOG_ERROR("table:%s, type:%d, err:%s", request.tbl().c_str()
            , request.type(), err.what());
    }
    return true;
}

bool MysqlDb::DBSelect(const std::string& sql, FUNC_DB_MAP_RET& fnCall, size_t& retNum)
{
    DBMap ret;
    return _DBSelect(sql, fnCall, ret, retNum, false);
}

bool MysqlDb::DBSelect(DbSelectReq& request, FUNC_DB_MAP_RET& fnCall, size_t& retNum)
{
    request.clear_ret();
    try
    {
        std::string strFields = _MakeFields(request.Fields());
        std::string strCon = _MakeCon(request.Con());
        _szQuery = "SELECT " + strFields + " FROM `" + request.tbl() + "` WHERE " + strCon + "; ";
        return _DBSelect(_szQuery, fnCall, request.Ret_Main(), retNum, false);
    }
    catch (const std::exception& err)
    {
        LOG_ERROR("table:%s, type:%d, err:%s", request.tbl().c_str()
            , request.type(), err.what());
    }
    return true;
}

bool MysqlDb::_DBSelect(const std::string& sql, FUNC_DB_MAP_RET& fnCall, DBMap& ret, size_t& retNum, bool bOne)
{
    mysqlpp::StoreQueryResult res;
    size_t retList = 0;
    for (size_t i = 0; i < 60; ++i)
    {
        try
        {
            mysqlpp::Query query = _pMysql->query(sql);
            if (!(res = query.store()))
            {
                int nErrNum = _pMysql->errnum();
                if (nErrNum == 0 || nErrNum == 1312)//没有返回集
                    return true;

                LOG_ERROR("SQL:%s error:%s, errono:%d, reconnect ts:%d", sql
                    , _GetDBLastError(), _pMysql->errnum(), i);
                if (_IsNetError(query.errnum()))
                {
                    if (!DbPing())
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
                else
                    return false;
            }
            else
            {
                ++retList;
                retNum = _FetchRows(res, ret);
                if (fnCall != nullptr) fnCall(ret);
                while (res = query.store_next())
                {
                    if (!bOne)
                    {
                        ++retList;
                        _FetchRows(res, ret);
                        if (fnCall != nullptr) fnCall(ret);
                    }
                }
                if (retList > 1) retNum = retList;
                return true;
            }
        }
        catch (mysqlpp::Exception& e)
        {
            LOG_ERROR("Excute SQL:%s failed! error:%s, err:%s, errono:%d, reconnect ts:%d"
                , sql, _GetDBLastError(), e.what(), _pMysql->errnum(), i);
            if (!DbPing())
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    return false;
}

bool MysqlDb::DbCallProc(const std::string& sql)
{
    FUNC_DB_MAP_RET fnCall = [&](DBMap& ret)->bool { return true; };
    size_t nRetNum = 0;
    DBMap retMap;
    return _DBSelect(sql, fnCall, retMap, nRetNum, true);
}

int64 MysqlDb::DbExec(const std::string& sql, eDBOptType queryType)
{
    for (size_t i = 0; i < 60; ++i)
    {
        int64 affectedNumRows = 0;
        try
        {
            mysqlpp::SimpleResult res;
            mysqlpp::Query query = _pMysql->query();
            query << sql;

            res = query.execute();
            if (!res)
            {
                int nErrNum = _pMysql->errnum();
                if (nErrNum == 0 || nErrNum == 1312)//没有返回集
                    return 0;

                affectedNumRows = -1;
                LOG_ERROR("Excute SQL:%s failed! error:%s, errono:%d, reconnect ts:%d"
                    , sql, query.error(), query.errnum(), i);

                if (_IsNetError(query.errnum()))
                {
                    if (!DbPing())
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    }
                }
                else
                {
                    return affectedNumRows;
                }
            }
            else
            {
                affectedNumRows = (int64) res.rows();
                return affectedNumRows;
            }
        }
        catch (mysqlpp::Exception& e)
        {
            LOG_ERROR("Excute SQL:%s failed! error:%s, err:%s, errono:%d, reconnect ts:%d"
                , sql, _GetDBLastError(), e.what(), _pMysql->errnum(), i);
            affectedNumRows = -1;

            if (!DbPing())
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    return -1;
}

int64 MysqlDb::DbExec(DbRequest& request)
{
    try
    {
        CHECK_LOG_RETVAL(MakeSqlStr(request), -1);
        return DbExec(GetSqlStr(), request.type());
    }
    catch (const std::exception& err)
    {
        LOG_ERROR("table:%s, type:%d, err:%s", request.tbl().c_str()
            , request.type(), err.what());
    }
    return 0;
}

bool MysqlDb::DbExec(std::list<std::string>& lstCreateSql)
{
    for (auto& itr : lstCreateSql)
    {
        string strSql = itr;
        strSql = trim_s(strSql);
        if (strSql.empty())
        {
            LOG_WARNING("find empty sql statement");
            continue;
        }
        DbExec(strSql, eDBOptType::NONE);
    }
    return true;
}

const char* MysqlDb::_GetDBLastError()
{
    return _pMysql->error();
}

const std::string& MysqlDb::_GetLastQuery()
{
    return _szQuery;
}

bool MysqlDb::DbUpdate(DbRequest& request)
{
    try
    {
        string strField = _MakeUpdate(request.reqMap());
        CHECK_LOG_RETVAL(!strField.empty(), false);

        string strCon = _MakeCon(request.Con());
        CHECK_LOG_RETVAL(!strCon.empty(), false);

        _szQuery = "UPDATE `" + request.tbl() + "` SET " + strField + " WHERE " + strCon + "; ";
        return true;
    }
    catch (const std::exception& err)
    {
        LOG_ERROR("table:%s, type:%d, err:%s", request.tbl().c_str()
            , request.type(), err.what());
    }
    return false;
}

bool MysqlDb::DbIncValue(DbRequest& request)
{
    try
    {
        string strInsert = _MakeIncInsert(request.Con(), request.reqMap());
        CHECK_LOG_RETVAL(!strInsert.empty(), false);

        string strUpdate = _MakeInc(request.reqMap());
        CHECK_LOG_RETVAL(!strUpdate.empty(), false);

        _szQuery = "INSERT INTO `" + request.tbl() + "` " + strInsert + " on duplicate key update " + strUpdate + "; ";
        return true;
    }
    catch (const std::exception& err)
    {
        LOG_ERROR("table:%s, type:%d, err:%s", request.tbl().c_str()
            , request.type(), err.what());
    }
    return false;
}

bool MysqlDb::DbInsert(DbRequest& request)
{
    try
    {
        string strField = _MakeInsert(request.reqMap());
        CHECK_LOG_RETVAL(!strField.empty(), false);

        _szQuery = "INSERT INTO `" + request.tbl() + "` " + strField + "; ";
        return true;
    }
    catch (const std::exception& err)
    {
        LOG_ERROR("table:%s, type:%d, err:%s", request.tbl().c_str()
            , request.type(), err.what());
    }
    return false;
}

bool MysqlDb::DbDelete(DbRequest& request)
{
    try
    {
        string strCon = _MakeCon(request.Con());
        CHECK_LOG_RETVAL(!strCon.empty(), false);

        _szQuery = "DELETE FROM `" + request.tbl() + "` WHERE " + strCon + ";";
        return true;
    }
    catch (const std::exception& err)
    {
        LOG_ERROR("table:%s, type:%d, err:%s", request.tbl().c_str()
            , request.type(), err.what());
    }
    return false;
}

bool MysqlDb::DbInsertUpdate(DbRequest& request)
{
    try
    {
        string strInsert = _MakeInsert(request.Con(), request.reqMap());
        CHECK_LOG_RETVAL(!strInsert.empty(), false);

        string strUpdate = _MakeUpdate(request.reqMap());
        CHECK_LOG_RETVAL(!strUpdate.empty(), false);

        _szQuery = "INSERT INTO `" + request.tbl() + "` " + strInsert + " on duplicate key update " + strUpdate + ";";
        return true;
    }
    catch (const std::exception& err)
    {
        LOG_ERROR("table:%s, type:%d, err:%s", request.tbl().c_str()
            , request.type(), err.what());
    }
    return false;
}

bool MysqlDb::MakeSqlStr(DbRequest& request)
{
    switch (request.type())
    {
        case eDBOptType::UPDATE:
            return DbUpdate(request);
            break;
        case eDBOptType::DELETE_X:
            return DbDelete(request);
            break;
        case eDBOptType::INSERT_UPDATE:
            return DbInsertUpdate(request);
            break;
        case eDBOptType::INSERT:
            return DbInsert(request);
            break;
        case eDBOptType::INC:
            return DbIncValue(request);
            break;
        default:
            LOG_ERROR("other op type : %d", request.type());
            break;
    }
    return false;
}

const std::string& MysqlDb::GetSqlStr()
{
    return _szQuery;
}

bool MysqlDb::_StreamItem(bool& isFirst, std::string& szName, std::string& szValue, umap < std::string, DBMap::DBItemNum>& mapData)
{
    for (auto iter = mapData.begin(); iter != mapData.end(); ++iter)
    {
        if (!isFirst)
        {
            szName += ',';
            szValue += ',';
        }
        isFirst = false;
        szName += iter->first;
        szValue += std::to_string(iter->second.value);
    }
    return isFirst;
}

bool MysqlDb::_StreamItem(bool& isFirst, std::string& szName, std::string& szValue, umap< std::string, DBMap::DBItemStr>& mapData)
{
    std::string szTemp;
    for (auto iter = mapData.begin(); iter != mapData.end(); ++iter)
    {
        if (!isFirst)
        {
            szName += ',';
            szValue += ',';
        }
        isFirst = false;

        szTemp.clear();
        mysqlpp::DBDriver::escape_string_no_conn(&szTemp, iter->second.value.c_str(), iter->second.value.length());

        szName += iter->first;
        szValue = szValue + '\'' + szTemp + '\'';
    }
    return isFirst;
}

string MysqlDb::_MakeInsert(DBMap& mapData)
{
    std::string szName;
    std::string szValue;
    bool isFirst = true;

    isFirst = _StreamItem(isFirst, szName, szValue, mapData.MapNum());
    isFirst = _StreamItem(isFirst, szName, szValue, mapData.MapStr());

    std::string strRet = '(' + szName + ')';
    strRet = strRet + "VALUES(" + szValue + ')';

    return strRet;
}

string MysqlDb::_MakeInsert(DBMap& mapCon, DBMap& mapReq)
{
    std::string szName;
    std::string szValue;
    bool isFirst = true;

    isFirst = _StreamItem(isFirst, szName, szValue, mapCon.MapNum());
    isFirst = _StreamItem(isFirst, szName, szValue, mapCon.MapStr());

    isFirst = _StreamItem(isFirst, szName, szValue, mapReq.MapNum());
    isFirst = _StreamItem(isFirst, szName, szValue, mapReq.MapStr());

    std::string strRet = '(' + szName + ')';
    strRet = strRet + "VALUES(" + szValue + ')';

    return strRet;
}

string MysqlDb::_MakeUpdate(DBMap& mapData)
{
    std::string szValue;

    bool bFirst = true;
    std::string szTemp;
    for (auto iter = mapData.MapNum().begin(); iter != mapData.MapNum().end(); ++iter)
    {
        if (!bFirst) szValue += ',';

        bFirst = false;
        _MakeOpType(szValue, iter->first, iter->second.value);
    }

    for (auto iter = mapData.MapStr().begin(); iter != mapData.MapStr().end(); ++iter)
    {
        if (!bFirst) szValue += ',';

        bFirst = false;

        szTemp.clear();
        mysqlpp::DBDriver::escape_string_no_conn(&szTemp, iter->second.value.c_str(), iter->second.value.length());

        _MakeOpType(szValue, iter->first, szTemp);
    }
    return szValue;
}

string MysqlDb::_MakeIncInsert(DBMap& mapCon, DBMap& mapReq)
{
    std::string szName;
    std::string szValue;
    bool isFirst = true;
    isFirst = _StreamItem(isFirst, szName, szValue, mapCon.MapNum());
    isFirst = _StreamItem(isFirst, szName, szValue, mapCon.MapStr());

    isFirst = _StreamItem(isFirst, szName, szValue, mapReq.MapNum());

    std::string strRet = '(' + szName + ')';
    strRet = strRet + "VALUES(" + szValue + ')';
    return strRet;
}

string MysqlDb::_MakeInc(DBMap& mapData)
{
    std::string szValue;

    bool bFirst = true;
    for (auto iter = mapData.MapNum().begin(); iter != mapData.MapNum().end(); ++iter)
    {
        if (!bFirst) szValue += ',';

        bFirst = false;
        szValue = szValue + iter->first + '=' + iter->first + " + '";
        szValue = szValue + std::to_string(iter->second.value) + '\'';
    }

    return szValue;
}

string MysqlDb::_MakeFields(uset<std::string>& vFields)
{
    std::string ss;
    bool bFirst = true;
    for (auto& item : vFields)
    {
        if (!bFirst) ss += ',';
        bFirst = false;
        ss += item;
    }
    return ss;
}

string MysqlDb::_MakeCon(DBMap& conditions)
{
    std::string szValue;
    std::string szTemp;

    bool isFirst = true;
    for (auto iter = conditions.MapNum().begin(); iter != conditions.MapNum().end(); ++iter)
    {
        if (!isFirst) szValue += " AND ";
        szValue += '(';
        _MakeOpType(szValue, iter->first, iter->second.value, iter->second.caltype);
        szValue += ')';
        isFirst = false;
    }

    for (auto iter = conditions.MapStr().begin(); iter != conditions.MapStr().end(); ++iter)
    {
        if (!isFirst) szValue += " AND ";

        szTemp.clear();
        mysqlpp::DBDriver::escape_string_no_conn(&szTemp, iter->second.value.c_str(), iter->second.value.length());

        szValue += '(';
        _MakeOpType(szValue, iter->first, szTemp, iter->second.caltype);
        szValue += ')';
        isFirst = false;
    }

    return szValue;
}

void MysqlDb::_MakeOpType(std::string& ss, const string& skey, const string& sValue, eDBEQType eType)
{
    switch (eType)
    {
        case eDBEQType::EQ:
            ss = ss + skey + "='" + sValue + '\'';
            break;
        case eDBEQType::GT:
            ss = ss + skey + ">'" + sValue + '\'';
            break;
        case eDBEQType::LT:
            ss = ss + skey + "<'" + sValue + '\'';
            break;
        case eDBEQType::GTE:
            ss = ss + skey + ">='" + sValue + '\'';
            break;
        case eDBEQType::LTE:
            ss = ss + skey + "<='" + sValue + '\'';
            break;
        case eDBEQType::NE:
            ss = ss + skey + "!='" + sValue + '\'';
            break;
        default:
            break;
    }
}

void MysqlDb::_MakeOpType(std::string& ss, const string& skey, const int64& sValue, eDBEQType eType /*= EDCT_EQ*/)
{
    switch (eType)
    {
        case eDBEQType::EQ:
            ss = ss + skey + "='" + std::to_string(sValue) + '\'';
            break;
        case eDBEQType::GT:
            ss = ss + skey + ">'" + std::to_string(sValue) + '\'';
            break;
        case eDBEQType::LT:
            ss = ss + skey + "<'" + std::to_string(sValue) + '\'';
            break;
        case eDBEQType::GTE:
            ss = ss + skey + ">='" + std::to_string(sValue) + '\'';
            break;
        case eDBEQType::LTE:
            ss = ss + skey + "<='" + std::to_string(sValue) + '\'';
            break;
        case eDBEQType::NE:
            ss = ss + skey + "!='" + std::to_string(sValue) + '\'';
            break;
        default:
            break;
    }
}

bool MysqlDb::MainLoop(time_t tmTick)
{
    if ((get_curr_time() - _tmLast) >= 60)
    {
        _tmLast = get_curr_time();
        return DbPing();
    }
    return true;
}

bool MysqlDb::DbPing()
{
    if (!_pMysql->ping())
    {
        ++_dwReconTimes;
        LOG_ERROR("failed! recon ts:%u", _dwReconTimes);
        if (DbConnect())
        {
            _dwReconTimes = 0;
            return true;
        }
        else
            return false;
    }
    return true;
}

bool MysqlDb::UpdateDBFile(const char* pszFileName, const char* pszDBName
    , int nDBId, const char* pszTableName/* = NULL*/)
{
    assert(pszFileName);
    assert(pszDBName);
    _strName = pszDBName;
    std::list<string> lstSqlData;
    if (!_OpenCreateDBFile(pszFileName, lstSqlData, nDBId, pszTableName))
    {
        LOG_ERROR("open %s file error", pszFileName);
        return false;
    }
    return DbExec(lstSqlData);
}

bool MysqlDb::UpdateLogFile(const char* pszFileName, const std::string& strDate)
{
    assert(pszFileName);
    std::list<string> lstSqlData;
    if (!_OpenLogDBFile(pszFileName, lstSqlData, strDate))
    {
        LOG_ERROR("open %s file error", pszFileName);
        return false;
    }
    return DbExec(lstSqlData);
}

bool MysqlDb::_IsNetError(int nErr)
{
    if (nErr == 1042 || nErr == 1043
        || nErr == 1081 || (nErr >= 1158 && nErr <= 1162)
        || nErr >= 2000 || nErr == 1218 || nErr == 1430)
    {
        return true;
    }
    return false;
}

bool MysqlDb::_OpenCreateDBFile(const string& strFileName, std::list<std::string>& lstCreateSql
    , int nDBId, const string& pszTableName)
{
    std::string strName = file::get_local_path(strFileName);
    FILE* pFile = fopen(strName.c_str(), "rb");
    if (NULL == pFile)
    {
        LOG_ERROR("open %s file error", strName.c_str());
        return false;
    }

    // get file size
    fseek(pFile, 0, SEEK_END);
    size_t nFileLen = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    char* pszBuff = new char[nFileLen + 1];
    memset(pszBuff, 0, nFileLen + 1);

    char* pszContent = new char[nFileLen + 256];
    memset(pszContent, 0, nFileLen + 256);

    fread(pszBuff, nFileLen, 1, pFile);
    fclose(pFile);

    if (!pszTableName.empty())
        sprintf_safe(pszContent, nFileLen + 128, pszBuff, _strName.c_str(), _strName.c_str(), nDBId, pszTableName);
    else
        sprintf_safe(pszContent, nFileLen + 128, pszBuff, _strName.c_str(), _strName.c_str(), nDBId);

    if (char* pDelim = strrchr(pszContent, ';'))
    {
        *pDelim = '\0';
    }

    str_split(pszContent, ';', lstCreateSql);
    delete[] pszContent;
    delete[] pszBuff;

    return true;
}

bool MysqlDb::_OpenLogDBFile(const string& strFileName
    , std::list<std::string>& lstCreateSql, const string& strDate)
{
    std::string strName = file::get_local_path(strFileName);
    FILE* pFile = fopen(strName.c_str(), "rb");
    if (NULL == pFile)
    {
        LOG_ERROR("open %s file error", strName.c_str());
        return false;
    }

    // get file size
    fseek(pFile, 0, SEEK_END);
    size_t nFileLen = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    char* pszBuff = new char[nFileLen + 1];
    memset(pszBuff, 0, nFileLen + 1);

    fread(pszBuff, nFileLen, 1, pFile);
    fclose(pFile);

    std::string strReplace;
    strReplace.assign(pszBuff, nFileLen);

    strReplace = str_replace(strReplace, "[date]", strDate);

    str_split(strReplace, ';', lstCreateSql, true);
    delete[] pszBuff;

    return true;

}
