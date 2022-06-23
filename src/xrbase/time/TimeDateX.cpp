/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "TimeDateX.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <chrono>

#include "time_func_x.h"
#include "string/str_split.h"
#include "string/str_num.h"

const char* szfmt[(size_t)ETimeStrType::END] = {
    "%Y-%m-%d %H:%M:%S"
    , "%Y-%m-%d"
    , "%H:%M:%S" 
    , "%Y%m%d%H%M%S"
    , "%Y%m%d"
    , "%H%M%S"
};

TimeDateX::TimeDateX(void)
: _timevalue(get_curr_time())
{
     localtime_s(&_tmstruct, &_timevalue);
}

TimeDateX::~TimeDateX(void)
{
}

std::string TimeDateX::Str(ETimeStrType etype)
{
    char ch[20];
    std::strftime(ch, 20, szfmt[(size_t)etype], &_tmstruct);
    return ch;
}

std::string TimeDateX::Str_s(ETimeStrType etype, time_t tmNow)
{
    std::time_t tmvalue = tmNow;

    if(tmvalue == 0)
        tmvalue = get_curr_time();
    std::tm tmsturct;
    localtime_s(&tmsturct, &tmvalue);

    if (etype == ETimeStrType::DAY_SECS)
    {
        return std::to_string(tmsturct.tm_hour * 3600 + tmsturct.tm_min * 60 + tmsturct.tm_sec);
    }

    char ch[20];
    std::strftime(ch, 20, szfmt[(size_t)etype], &tmsturct);
    return ch;
}

int64_t TimeDateX::Number(ETimeStrType etype)
{
    if (etype == ETimeStrType::DAY_SECS)
    {
        return (int64_t) _tmstruct.tm_hour*3600 + (int64_t)_tmstruct.tm_min* 60 + (int64_t) _tmstruct.tm_sec;
    }

    std::string strs = Str(etype);
    return stoll_x(strs);
}

int64_t TimeDateX::Number_s(ETimeStrType etype, time_t tmNow)
{
    std::string strs = Str_s(etype, tmNow);
    return stoll_x(strs);
}

void TimeDateX::SetNow()
{
	_timevalue = get_curr_time();
    localtime_s(&_tmstruct, &_timevalue);
}

void TimeDateX::SetTime(const std::time_t& SurTime)
{
	_timevalue = SurTime;
    localtime_s(&_tmstruct, &_timevalue);
}

std::time_t TimeDateX::GetTime()
{
	return _timevalue;
}

bool TimeDateX::ConvTime_s(time_t& tmRet, const std::string& strAllTime)
{
    tmRet = 0;
    bool bRet = true;
	struct std::tm tm1;
    std::vector<std::string> vDate;
    str_split(strAllTime, ' ', vDate);
    if (vDate.size() < 2)
    {
        LOG_ERROR("error! date:%s", strAllTime.c_str());
        return false;
    }
    else
    {
        std::vector<int> vnDate, vnTime;
        str_split_num(vDate[0], '-', vnDate);
        str_split_num(vDate[1], ':', vnTime);
        if (vnDate.size() < 3 || vnTime.size() < 3)
        {
            LOG_ERROR("error! date:%s", strAllTime.c_str());
            tm1.tm_year = 0;
            tm1.tm_mon = 0;
            tm1.tm_hour = 0;
            tm1.tm_isdst = 0;
        }
        else
        {
            tm1.tm_year = vnDate[0] - 1900;
            tm1.tm_mon = vnDate[1] - 1;
            tm1.tm_mday = vnDate[2];

            tm1.tm_hour = vnTime[0];
            tm1.tm_min = vnTime[1];
            tm1.tm_sec = vnTime[2];
        }
    }
	tmRet = mktime(&tm1);
	return bRet;
}

bool TimeDateX::ConvDate_s(time_t& tmRet, const std::string& strDate, const char cSplit)
{
    tmRet = 0;
    if (strDate.length() == 0)
        return false;

    bool bRet = true;
	struct std::tm tm1;
    std::vector<int> vDate;
    str_split_num(strDate, cSplit, vDate);
    if (vDate.size() < 3)
    {
        LOG_ERROR("error! date:%s", strDate.c_str());
        return false;
    }
    else
    {
        tm1.tm_year = vDate[0] - 1900;
        tm1.tm_mon = vDate[1] - 1;
        tm1.tm_mday = vDate[2];

        tm1.tm_hour = 0;
        tm1.tm_min = 0;
        tm1.tm_sec = 0;
        tm1.tm_isdst = 0;
    }
	tmRet = mktime(&tm1);
	return bRet;
}

uint32_t TimeDateX::ConvTimeSec_s(const std::string& strTime)
{
    std::vector<uint32_t> vDate;
    str_split_num(strTime, ':', vDate);
    if (vDate.size() < 3)
    {
        LOG_ERROR("error! date:%s", strTime.c_str());
        return 0;
    }
    uint32_t ret = vDate[0] * 3600 + vDate[1] * 60 + vDate[2];
    return ret;
}