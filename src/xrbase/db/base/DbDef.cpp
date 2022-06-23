/*
 *
 *      Author: venture
 */
#include "DbDef.h"

bool DBMap::HasField(const std::string& strFiled)
{
    if (_nums.find(strFiled) != _nums.end())
        return true;
    if (_strs.find(strFiled) != _strs.end())
        return true;
    if (_lstNum.find(strFiled) != _lstNum.end())
        return true;
    if (_lstStr.find(strFiled) != _lstStr.end())
        return true;
    return false;
}

eDBFieldType DBMap::GetFieldType(const std::string& strFiled)
{
    if (_nums.find(strFiled) != _nums.end())
        return eDBFieldType::INTEGER;
    if (_strs.find(strFiled) != _strs.end())
        return eDBFieldType::STRING;
    if (_lstNum.find(strFiled) != _lstNum.end())
        return eDBFieldType::LIST;
    if (_lstStr.find(strFiled) != _lstStr.end())
        return eDBFieldType::LIST;
    return eDBFieldType::NONE;
}

void DBMap::clear()
{
    _strs.clear();
    _nums.clear();
    _lstNum.clear();
    _lstStr.clear();
}

bool DBMap::empty()
{
    return _strs.size() == 0 && _nums.size() == 0
        && _lstNum.size() == 0 && _lstStr.size() == 0;
}

void DBMap::AddBinary(const std::string& key, const std::string& value, eDBEQType eCalType)
{
    DBItemStr& str = _strs[key];
    str.type = eDBFieldType::BINARY;
    str.value = value;
    str.caltype = eCalType;
}

void DBMap::AddList(const std::string& key, const std::list<int64_t>& lstNum)
{
    _lstNum[key] = lstNum;
}

void DBMap::AddListStr(const std::string& key, const std::list<std::string>& lstStr)
{
    _lstStr[key] = lstStr;
}

void DBMap::AddDouble(const std::string& key, const double& value, eDBEQType eCalType /*= EDCT_EQ*/)
{
    DBItemNum& item = _nums[key];
    item.caltype = eCalType;
    item.dbl = value;
    item.type = eDBFieldType::DOUBLE;
}

void DBMap::AddStr(const std::string& key, const std::string& value, eDBEQType eCalType)
{
    auto itr = _strs.find(key);
    DBItemStr& str = _strs[key];
    str.type = eDBFieldType::STRING;
    str.value = value;
    str.caltype = eCalType;
}

void DBMap::AddTime(const std::string& key, const time_t& value, eDBEQType eCalType)
{
    DBItemNum& num = _nums[key];
    num.type = eDBFieldType::TIME;
    num.value = value;
    num.caltype = eCalType;
}

void DBMap::AddTime(const std::string& key, const std::chrono::seconds& value, eDBEQType eCalType)
{
    AddTime(key, value.count(), eCalType);
}

const std::string& DBMap::Str(const std::string& key, const std::string& def /*= ""*/)
{
    auto itr = _strs.find(key);
    if (itr != _strs.end())
    {
        return itr->second.value;
    }
    return def;
}

const std::string& DBMap::Binary(const std::string& key, const std::string& def /*= ""*/)
{
    auto itr = _strs.find(key);
    if (itr != _strs.end())
    {
        return itr->second.value;
    }
    return def;
}

const time_t DBMap::Time(const std::string& key, const time_t& def)
{
    auto itr = _nums.find(key);
    if (itr != _nums.end())
        return (time_t) (itr->second.value);
    return def;
}

const double DBMap::Double(const std::string& key, const double& def)
{
    auto itr = _nums.find(key);
    if (itr != _nums.end())
        return itr->second.dbl;
    return def;
}

const float DBMap::Float(const std::string& key, const float& def)
{
    return (float) Double(key, def);
}

void DBRet::clear()
{
    _retMain.clear();
    _retVec.clear();
    _lstFileds.clear();
}

void DBReqBase::AddObjField(const std::string& sectable, const std::string& field)
{
    AddField(field, sectable + ".");
}

void DBReqBase::AddlistField(const std::string& sectable, const std::string& field)
{
    AddField(field, sectable + ".$.");
}

void DBReqBase::Init(eDBOptType type, const std::string& strName
    , const std::string& strSecName, eDBSecKeyType secType)
{
    _eQueryType = type;
    _tblName = strName;
    _secTable._secTblName = strSecName;
    _secTable._secType = secType;
    if (_secTable._secType == eDBSecKeyType::NONE && strSecName.length() > 0)
    {
        _secTable._secType = eDBSecKeyType::LIST;
    }
}

void DBReqBase::InitSecTbl(const std::string& strSecName, eDBSecKeyType secType)
{
    _secTable._secTblName = strSecName;
    if (strSecName.empty())
        _secTable._secType = eDBSecKeyType::NONE;
    else
        _secTable._secType = secType;
}

void DBReqBase::clear()
{
    //     m_secTable.clear();
    //     m_tblName.clear();		//表名
    //     m_eQueryType = eDB_None;	//执行类型
    _conditions.clear();
    _vFields.clear();
}

void DBReqBase::clear_sectbl()
{
    _secTable.clear();
}

bool DBReqBase::CheckKey(int nCheckType)
{
    if (nCheckType & (int) eCheckKeyType::KEY)
    {
        if (_tblName.empty())
            return true;
        else
        {
            if (!_conditions.HasField(F_KEY))
                return false;
        }
    }

    if (nCheckType & (int) eCheckKeyType::SEC_KEY)
    {
        if (_secTable.type() == eDBSecKeyType::LIST)
        {
            return _conditions.HasField(S_KEY);
        }
    }
    return true;
}

DbRequest::DbRequest(const DbRequest& req)
{
    _reqFields = req._reqFields;
    _secTable = req._secTable;
    _tblName = req._tblName;		//表名
    _eQueryType = req._eQueryType;	//执行类型
    _conditions = req._conditions;	//条件
    _vFields = req._vFields;      //查找、删除的字段
    _bEmptyError = req._bEmptyError;
}

DbRequest& DbRequest::operator=(const DbRequest& req)
{
    _reqFields = req._reqFields;
    _secTable = req._secTable;
    _tblName = req._tblName;		//表名
    _eQueryType = req._eQueryType;	//执行类型
    _conditions = req._conditions;	//条件
    _vFields = req._vFields;      //查找、删除的字段
    _bEmptyError = req._bEmptyError;
    return *this;
}

void SecTblDesc::clear()
{
    _secTblName.clear();
    _secType = eDBSecKeyType::NONE;
    _secLimit = 0;
}
