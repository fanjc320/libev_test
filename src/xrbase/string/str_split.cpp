/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "dtype.h"
#include "str_split.h"
#include "log/LogMgr.h"

size_t str_split_set(const std::string& strSource, char cSplit, 
            uset<std::string> &setRetString, bool bTrim /*= false*/, bool bShowError)
{
    setRetString.clear();
    std::list<std::string> lstRet;
    str_split(strSource, cSplit, lstRet, bTrim, bShowError);
    setRetString.insert(lstRet.begin(), lstRet.end());
    return setRetString.size();
}

size_t str_split_float(const std::string& strSource, char cSplit, std::list<double> &lstRetfloat)
{
    lstRetfloat.clear();
    std::list<std::string> lstStr;
    str_split(strSource, cSplit, lstStr);
    double val = 0.0;
    for (const auto& itr : lstStr)
    {
        val = stod_x(itr);
        lstRetfloat.push_back(val);
    }
    return lstRetfloat.size();
}
