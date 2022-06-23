/*
 *
 *      Author: venture
 */

#include "pch.h"
#include <iostream>

#include "HttpParam.h"
#include "string/str_num.h"

HttpParam::HttpParam()
{
    // TODO Auto-generated constructor stub

}

HttpParam::~HttpParam()
{
    // TODO Auto-generated destructor stub
}

void HttpParam::AddParam(std::string strIndex, std::string strValue)
{
    std::string str = strIndex + "=" + strValue;
    _vecParam.push_back(str);
    _mapParam[strIndex] = strValue;
}

void HttpParam::AddParam(std::string strIndex, int64_t value)
{
    std::string str = strIndex + "=";
    std::string strValue = std::to_string(value);
    str = str + std::to_string(value);
    _vecParam.push_back(str);
    _mapParam[strIndex] = strValue;
}

std::string HttpParam::GetParam(std::string strIndex)
{
    auto itr = _mapParam.find(strIndex);
    if (itr != _mapParam.end())
        return itr->second;
    return "";
}

int64_t HttpParam::GetParamInt(std::string strIndex)
{
    std::string strRet = GetParam(strIndex);
    return stoll_x(strRet);
}

void HttpParam::Clear()
{
    _vecParam.clear();
    _mapParam.clear();
    _strRet.clear();
    _qwKey = 0; //key
    _bPost = false;
    _bRet = false;
    _strFile.clear();
    _retCode = 0;
}

std::string HttpParam::GetUrl()
{
    std::string str = GetHost() + GetHead();
    if (_bPost)
        return str;

    str += "?";
    str += GetFields();
    return str;
}

std::string HttpParam::GetFields()
{
    std::string str;
    for (size_t i = 0; i < _vecParam.size(); ++i)
    {
        str.append(_vecParam[i]);
        if ((i + 1) < _vecParam.size())
        {
            str.append("&");
        }
    }
    //std::cout << str << std::endl;
    return str;
}