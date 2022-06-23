/*
 *
 *      Author: venture
 */

#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include "dtype.h"

class HttpParam
{
public:
    HttpParam();
    virtual ~HttpParam();

public:
    std::string GetUrl();

    const std::string& GetHost() const { return _strHost; }
    void SetHost(std::string val) { _strHost = val; }
    const std::string& GetHead() const { return _strHead; }
    void SetHead(std::string val) { _strHead = val; }

    void AddParam(std::string strIndex, std::string strValue);
    void AddParam(std::string strIndex, int64_t value);

    std::string GetParam(std::string strIndex);
    int64_t GetParamInt(std::string strIndex);

    void Clear();

public:
    int64_t GetKey() const { return _qwKey; }
    void SetKey(int64_t val) { _qwKey = val; }
    bool IsPost() const { return _bPost; }
    void SetPost(bool val) { _bPost = val; }
    bool IsRet() const { return _bRet; }
    void SetRet(bool val) { _bRet = val; }

    std::string& GetStrRet() { return _strRet; }
    void SetStrRet(std::string val) { _strRet = val; }

    const std::string& GetFile() const { return _strFile; }
    void SetFile(std::string val) { _strFile = val; }
    int32_t GetRetCode() const { return _retCode; }
    void SetRetCode(int32_t val) { _retCode = val; }

    std::string GetFields();
    const umap<std::string, std::string>& GetMapFields() { return _mapParam; }

    void AddErrTimes() { ++_errTimes; }
    int32_t GetErrTimers() { return _errTimes; }

protected:    
    std::string _strHost;
    std::string _strHead;

    int64_t _qwKey = 0; //key
    std::string _strFile;
    std::vector<std::string> _vecParam;
    umap<std::string, std::string> _mapParam;

    std::string _strRet;
    int32_t     _retCode = 0;
    bool        _bRet = false;
    bool        _bPost = false;
    int32_t     _errTimes = 0;
};
