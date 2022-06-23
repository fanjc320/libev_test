/*
 *
 *      Author: venture
 */
#pragma once

#include <string>

#include "log/LogMgr.h"

inline double stod_x(const std::string& _Str)
{
    if (_Str.empty()) 
        return 0.f;
    try
    {
        return std::stod(_Str);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s-%s", _Str.c_str(), e.what());
        return 0.f;
    }
}

//支持2-36进制
inline std::string tostring_x(int64_t x, int radix)
{
    if (radix == 0) return "";

    std::string strDest;
    strDest.reserve(24);
    int64_t i = 0, n, s;
    if (x < 0)
        n = -x;
    else
        n = x;
    while (n > 0)
    {
        s = n % radix;
        n = n / radix;
        if (s > 9)
            strDest.insert(strDest.begin(), 1, char('a' + s - 10));
        else
            strDest.insert(strDest.begin(), 1, char('0' + s));
    }
    if (x < 0)
        strDest.insert(strDest.begin(), 1, '-');

    return strDest;
}

//支持2-36进制
inline int64_t stoll_x(const std::string& _Str, int radix = 10)
{
    if (_Str.empty())
        return 0;
    try
    {
        return std::stoll(_Str, nullptr, radix);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s-%s", _Str.c_str(), e.what());
        return 0;
    }
}
//支持2-36进制
inline int32_t stoi_x(const std::string& _Str, int radix = 10)
{
    if (_Str.empty())
        return 0;
    try
    {
        return std::stoi(_Str, nullptr, radix);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s-%s", _Str.c_str(), e.what());
        return 0;
    }
}
