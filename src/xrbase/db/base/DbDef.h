/*
 *
 *      Author: venture
 */
#pragma once

#include <string>
#include <dtype.h>
#include <functional>
#include <vector>
#include <list>
#include <chrono>
#include "dtype.h"

class DBRet;
class DbSelectReq;
class DbRequest;
class DBMap;

typedef std::function<bool(DBRet&)> FUNC_DB_RET;
typedef std::function<bool(DBMap&)> FUNC_DB_MAP_RET;

//数据库执行类别
enum class eDBOptType : int8_t
{
    NONE = 0,
    SELECT,
    INSERT,
    UPDATE,
    DELETE_X, //慎用，删除一条数据
    DROP_DARAM, //删除一级列表的单个字段
    INSERT_UPDATE,
    INSERT_LOG, //日志专用
    INC, //针对单字段自增(正数),自减(负数)
    UPDATE_MANY, //慎用，用于更新所有人的数据状态的
    STRING, //用于执行组装好的字符串
    DROP, //慎用，删除表
};

//数据库字段的比较查询, 只支持一级的字段
enum class eDBEQType : int8_t
{
    EQ = 0, //等于
    GT = 1, //大于
    LT = 2, //小于
    GTE = 3, //大于等于
    LTE = 4, //小于等于
    NE = 5,  //不等于
    END,
};
enum class eDBFieldType : int8_t
{
    NONE = 0,
    INTEGER = 1,
    TIME,
    STRING,
    BINARY,
    DOUBLE,
    LIST,
};

//数据操作二级项的类型
enum class eDBSecKeyType : int8_t
{
    NONE,//无二级
    OBJECT,//二级为数据项
    LIST,//二级为列表
};

enum class eCheckKeyType : int8_t
{
    KEY = 1,
    SEC_KEY = 2,
    DOUBLE_KEY = 3
};

class DBMap
{
public:
    struct DBItemNum
    {
        eDBFieldType type = eDBFieldType::INTEGER;
        eDBEQType    caltype = eDBEQType::EQ;
        union
        {
            int64_t      value = 0; // 数值，时间
            double       dbl;
        };
        DBItemNum() { }
        DBItemNum(eDBFieldType eType, int64_t eValue) : type(eType), value(eValue) { }
    };
    struct DBItemStr
    {
        eDBFieldType type = eDBFieldType::STRING;
        eDBEQType  caltype = eDBEQType::EQ;
        std::string value;
        DBItemStr() { }
        DBItemStr(eDBFieldType eType, std::string eValue) : type(eType), value(eValue) { }
    };

protected:    
    umap<std::string, DBItemNum>                _nums; //返回字段
    umap<std::string, std::list<int64_t>>       _lstNum;

    umap<std::string, DBItemStr>                _strs;
    umap<std::string, std::list<std::string>>   _lstStr;

public:
    ~DBMap(void) { }
    DBMap() { }

    bool HasField(const std::string& strFiled);
    eDBFieldType GetFieldType(const std::string& strFiled);

    void clear();
    bool empty();

    umap<std::string, std::list<int64_t>>& MapList() { return _lstNum; }
    umap<std::string, std::list<std::string>>& MapListStr() { return _lstStr; }

    umap<std::string, DBItemStr>& MapStr() { return _strs; }
    umap<std::string, DBItemNum>& MapNum() { return _nums; }

    //设置
    void AddBinary(const std::string& key, const std::string& value, eDBEQType eCalType = eDBEQType::EQ);

    template <typename Iterator>
    void AddList(const std::string& key, const Iterator& begin, const Iterator& end)
    {
        _lstNum[key].clear();
        _lstNum[key].assign(begin, end);
    }

    void AddList(const std::string& key, const std::list<int64_t>& lstNum);

    template <typename Iterator>
    void AddListStr(const std::string& key, const Iterator& begin, const Iterator& end)
    {
        _lstStr[key].clear();
        _lstStr[key].assign(begin, end);
    }

    void AddListStr(const std::string& key, const std::list<std::string>& lstStr);
    void AddDouble(const std::string& key, const double& value, eDBEQType eCalType = eDBEQType::EQ);
    void AddStr(const std::string& key, const std::string& value, eDBEQType eCalType = eDBEQType::EQ);

    template <typename T>
    void AddInt(const std::string& key, const T& value, eDBEQType eCalType = eDBEQType::EQ)
    {
        DBItemNum& num = _nums[key];
        num.type = eDBFieldType::INTEGER;
        num.value = (int64_t) value;
        num.caltype = eCalType;
    }

    void AddTime(const std::string& key, const time_t& value, eDBEQType eCalType = eDBEQType::EQ);
    void AddTime(const std::string& key, const std::chrono::seconds& value, eDBEQType eCalType = eDBEQType::EQ);

    //读取
    const std::list<int64_t>& List(const std::string& key, const std::list<int64_t>& def = {}) { MAP_FIND_DEF(_lstNum, key, def); }
    const std::list<std::string>& ListStr(const std::string& key, const std::list<std::string>& def = {}) { MAP_FIND_DEF(_lstStr, key, def); }

    const std::string& Str(const std::string& key, const std::string& def = "");
    const std::string& Binary(const std::string& key, const std::string& def = "");

    const time_t Time(const std::string& key, const time_t& def = 0);
    const double Double(const std::string& key, const double& def = 0.0f);
    const float Float(const std::string& key, const float& def = 0.0f);

    template <typename T>
    const T Num(const std::string& key, const T& def = 0)
    {
        auto itr = _nums.find(key);
        if (itr != _nums.end()) return (T) (itr->second.value);
        return def;
    }

    const int32_t NumInt(const std::string& key, const int32_t& def = 0) { return Num<int32_t>(key, def); }
    const uint32 NumUInt(const std::string& key, const uint32& def = 0) { return Num<uint32>(key, def); }
    const int64_t NumLong(const std::string& key, const int64_t& def = 0) { return Num<int64_t>(key, def); }
    const uint64 NumULong(const std::string& key, const uint64& def = 0) { return Num<uint64>(key, def); }
    const bool Bool(const std::string& key, const bool& def = false) { return Num<uint32>(key, def) != 0; }
};

typedef std::vector<DBMap> sDBSecRet;

class DBRet
{
protected:
    DBMap                           _retMain;	//返回一级字段
    umap<std::string, sDBSecRet>    _retVec;    //返回二级字段数组
    std::list<std::string>          _lstFileds; //每个字段的顺序
public:
    ~DBRet(void) { }

    void clear();
    bool empty() { return _retMain.empty() && _retVec.empty(); }

    //返回值    
    const std::list<int64_t>& List(const std::string& key, const std::list<int64_t>& def = {}) { return _retMain.List(key, def); }
    const std::string& Binary(const std::string& key, const std::string& def = "") { return _retMain.Binary(key, def); }
    const std::string& Str(const std::string& key, const std::string& def = "") { return _retMain.Str(key, def); }
    template <typename T>
    const T Num(const std::string& key, const T& def = 0) { return _retMain.Num(key, def); }
    const int32_t NumInt(const std::string& key, const int32_t& def = 0) { return _retMain.NumInt(key, def); }
    const uint32 NumUInt(const std::string& key, const uint32& def = 0) { return _retMain.NumUInt(key, def); }
    const int64_t NumLong(const std::string& key, const int64_t& def = 0) { return _retMain.NumLong(key, def); }
    const uint64 NumULong(const std::string& key, const uint64& def = 0) { return _retMain.NumULong(key, def); }
    const time_t Time(const std::string& key, const time_t& def = 0) { return _retMain.Time(key, def); }
    const bool Bool(const std::string& key, const bool& def = false) { return _retMain.Bool(key, def); }
    const double Double(const std::string& key, const double& def = false) { return _retMain.Double(key, def); }
    const float Float(const std::string& key, const float& def = false) { return _retMain.Float(key, def); }

    //返回
    DBMap& Ret_Main() { return _retMain; }
    sDBSecRet& Ret_Vec(const std::string& key) { return _retVec[key]; }
    umap<std::string, sDBSecRet>& Ret_Vec() { return _retVec; }
    bool HaveVecField(const std::string& key) { return _retVec.find(key) != _retVec.end(); }
    void AddField(const std::string& key) { _lstFileds.push_back(key); }
};

class SecTblDesc
{
protected:
    friend class    DBReqBase;
    eDBSecKeyType	_secType = eDBSecKeyType::NONE; //二级表的类型
    int32_t         _secLimit = 0; //负数:保存最后, 正数:保存前面
    std::string		_secTblName;	//二级表名

public:
    ~SecTblDesc(void) { }

    void clear();
    const std::string& name() { return _secTblName; }
    eDBSecKeyType type() { return _secType; }
    bool IsList() { return _secType == eDBSecKeyType::LIST; }
    bool IsObj() { return _secType == eDBSecKeyType::OBJECT; }
    bool IsEmpty() { return _secType == eDBSecKeyType::NONE; }

    const int32_t limit() { return _secLimit; }
};

class DBReqBase
{
protected:
    SecTblDesc              _secTable;
    eDBOptType		        _eQueryType = eDBOptType::NONE;	//执行类型
    std::string			    _tblName;		//表名
    DBMap				    _conditions;	//条件
    uset<std::string>	    _vFields;       //查找、删除的字段
public:
    virtual ~DBReqBase(void) { }

    const std::string F_KEY = "_id";
    const std::string S_KEY = "id";

    virtual void clear();

    //表名
    const std::string& tbl() { return _tblName; }
    void SetTbl(std::string& strTbl) { _tblName = strTbl; }

    SecTblDesc& sectbl() { return _secTable; }
    //负数:保存最后, 正数:保存前面
    void SetSecLimit(int32_t limit) { _secTable._secLimit = limit; }

    DBMap& Con() { return _conditions; }
    eDBOptType type() { return _eQueryType; }
    void SetType(eDBOptType type) { _eQueryType = type; }

    //条件
    void AddConStr(const std::string& key, const std::string& value, eDBEQType eCalType = eDBEQType::EQ) { _conditions.AddStr(key, value, eCalType); }
    template <typename T>
    void AddCon(const std::string& key, const T& value, eDBEQType eCalType = eDBEQType::EQ) { _conditions.AddInt(key, value, eCalType); }
    void AddConTime(const std::string& key, const time_t& value, eDBEQType eCalType = eDBEQType::EQ) { _conditions.AddTime(key, value, eCalType); }

    template <typename Iterator>
    void AddConLst(const std::string& key, const Iterator& begin, const Iterator& end)
    {
        _conditions.AddList(key, begin, end);
    }

    //字段
    void AddField(const std::string& field, const std::string& strHead = "") { _vFields.insert(strHead + field); }
    void AddField(const uset<std::string>& sfield) { _vFields.insert(sfield.begin(), sfield.end()); }
    void AddObjField(const std::string& sectable, const std::string& field);
    void AddlistField(const std::string& sectable, const std::string& field);

    uset<std::string>& Fields() { return _vFields; }
    //重置查询字段
    void reset_field() { _vFields.clear(); }
    void reset_con() { _conditions.clear(); }

    void Init(eDBOptType type, const std::string& strName
        , const std::string& strSecName = "", eDBSecKeyType secType = eDBSecKeyType::NONE);

    void InitSecTbl(const std::string& strSecName, eDBSecKeyType secType = eDBSecKeyType::LIST);
    void clear_sectbl();

    virtual bool CheckKey(int nCheckType = ((int)eCheckKeyType::DOUBLE_KEY));
};

//查找
class DbSelectReq : public DBReqBase
{
protected:
    DBRet			_retField;			//返回的结果数据

public:
    DbSelectReq(const std::string& strName = ""
        , const std::string& strSecName = ""
        , eDBSecKeyType secType = eDBSecKeyType::NONE)
    {
        DBReqBase::Init(eDBOptType::SELECT, strName, strSecName, secType);
    }
    virtual ~DbSelectReq(void) { }

    void Init(const std::string& strName, const std::string& strSecName = ""
        , eDBSecKeyType secType = eDBSecKeyType::NONE)
    {
        DBReqBase::Init(eDBOptType::SELECT, strName, strSecName, secType);
    }

    virtual void clear()
    {
        DBReqBase::clear();
        _retField.clear();
    }

    virtual void clear_ret()
    {
        _retField.clear();
    }

    //获得返回值
    DBRet& Ret() { return _retField; }
    DBMap& Ret_Main() { return _retField.Ret_Main(); }
    sDBSecRet& Ret_Vec(const std::string& key) { return (_retField.Ret_Vec())[key]; }
    umap<std::string, sDBSecRet>& Ret_Vec() { return _retField.Ret_Vec(); }

    //判断返回为空
    bool empty_ret() { return _retField.empty(); }
};

//修改，删除，增加
class DbRequest : public DBReqBase
{
protected:
    DBMap			_reqFields;	//处理字段
    bool            _bEmptyError = true; //删除的数据为空时报错

public:
    DbRequest(const DbRequest& req);
    DbRequest& operator = (const DbRequest& map);

    DbRequest(eDBOptType eType = eDBOptType::NONE
        , const std::string& strName = ""
        , const std::string& strSecName = "", eDBSecKeyType secType = eDBSecKeyType::NONE)
    {
        Init(eType, strName, strSecName, secType);
    }
    virtual ~DbRequest(void) { }

    bool ReqEmpty() { return _reqFields.empty(); }
    virtual void clear()
    {
        DBReqBase::clear();
        _reqFields.clear();
    }

    DBMap& reqMap() { return _reqFields; }
    //请求
    void AddReqStr(const std::string& key, const std::string& value) { _reqFields.AddStr(key, value); }
    template <typename T>
    void AddReq(const std::string& key, const T& value) { _reqFields.AddInt(key, value); }
    void AddReqDouble(const std::string& key, const double& value) { _reqFields.AddDouble(key, value); }

    void AddReqTime(const std::string& key, const time_t& value) { _reqFields.AddTime(key, value); }
    void AddReqBinary(const std::string& key, const std::string& value) { _reqFields.AddBinary(key, value); }

    template <typename Iterator>
    void AddReqlist(const std::string& key, const Iterator& begin, const Iterator& end)
    {
        _reqFields.AddList(key, begin, end);
    }

    template <typename Iterator>
    void AddReqlistStr(const std::string& key, const Iterator& begin, const Iterator& end)
    {
        _reqFields.AddListStr(key, begin, end);
    }

    bool IsDelError() { return _bEmptyError; }
    void SetDelError(bool bError) { _bEmptyError = bError; }
};