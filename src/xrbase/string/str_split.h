/*
 *      程序中允许用到的分割符号有下面六个
 *      # , : ; | -
 *      Author: venture
 */

#pragma once

#include <string>
#include <vector>
#include <list>
#include "string/str_num.h"
#include "platform/func_stack.h"
#include "dtype.h"

 // 按cSplit字符分割字符串, 支持，list, vector
template<typename T>
inline size_t str_split(const std::string& strSource, char cSplit
    ,T& lstRetString, bool bTrim = false
    , bool bShowError = true)
{
    lstRetString.clear();
    if (strSource.length() == 0)
    {
        return 0;
    }
    std::string strSub;
    size_t last = 0;
    size_t index = strSource.find_first_of(cSplit, last);
    while (index != std::string::npos)
    {
        strSub = strSource.substr(last, index - last);

        //空的会被过滤掉
        if (strSub.empty())
        {
            if (!bTrim)
            {
                if (bShowError)
                {
                    std::string strCall;
                    func_stack::stack(strCall);
                    LOG_ERROR(" %s, %s", strSource.c_str(), strCall.c_str());
                }
                lstRetString.push_back(strSub);
            }
        }
        else
            lstRetString.push_back(strSub);

        last = index + 1;
        index = strSource.find_first_of(cSplit, last);
    }
    if (strSource.size() - last > 0)
    {
        strSub = strSource.substr(last, strSource.size() - last);
        if (strSub.empty() && bShowError)
        {
            std::string strCall;
            func_stack::stack(strCall);
            LOG_ERROR(" %s, %s", strSource.c_str(), strCall.c_str());
        }
        lstRetString.push_back(strSub);
    }

    return lstRetString.size();
}

// 按cSplit字符分割字符串
extern size_t str_split_set(const std::string& strSource, char cSplit
    , uset<std::string>& setRetString, bool bTrim = false
    , bool bShowError = true);

// 按cSplit字符分割字符串
extern size_t str_split_float(const std::string& strSource, char cSplit
    , std::list <double>& lstRetfloat);

// 按cSplit字符分割字符串, list, vector
template<typename T>
inline size_t str_split_num(const std::string& strSource, char cSplit, std::vector<T>& lstRetInt)
{
    static_assert(std::is_integral<T>::value, "Type T must be intergral!");

    lstRetInt.clear();
    std::list<std::string> lstStr;
    str_split(strSource, cSplit, lstStr);
    T val = 0;
    for (const auto& itr : lstStr)
    {
        val = static_cast<T>(stoll_x(itr));
        lstRetInt.push_back(val);
    }
    return lstRetInt.size();
}

// 按cSplit字符分割字符串
template<typename T>
inline size_t str_split_num_set(const std::string& strSource, char cSplit, uset<T>& sRetInt)
{
    static_assert(std::is_integral<T>::value, "Type T must be intergral!");

    sRetInt.clear();
    std::list<std::string> lstStr;
    str_split(strSource, cSplit, lstStr);
    T val = 0;
    for (const auto& itr : lstStr)
    {
        val = static_cast<T>(stoll_x(itr));

        sRetInt.insert(val);
    }
    return sRetInt.size();
}

//按照指定分隔符生成字符串, 支持字符串和数字
template <typename Iterator>
inline std::string con_to_str(Iterator begin, Iterator end, char cSplit = ':')
{
    std::string ss;
    bool bFirst = true;
    for (auto itr = begin; itr != end; ++itr)
    {
        if (!bFirst) ss += cSplit;

        ss += *itr;
        bFirst = false;
    }

    return ss;
}

//按照指定分隔符生成字符串, 支持字符串和数字
template <typename Iterator>
inline std::string map_to_str(Iterator begin, Iterator end, char cHighSplit = ';', char cLowSplit = ':')
{
    std::string ss;
    bool bFirst = true;
    for (auto itr = begin; itr != end; ++itr)
    {
        if (!bFirst)  ss += cHighSplit;

        ss += itr->first;
        ss += cLowSplit;
        ss += itr->second;

        bFirst = false;
    }
    return ss;
}