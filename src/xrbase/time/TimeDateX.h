/*
 *
 *      Author: venture
 */
#pragma once
#include <string.h>
#include <stdlib.h>
#include <exception>
#include <stdexcept>
#include <ctime>

enum class ETimeStrType : int8_t
{
    ALL,    //2010-01-01 01:01:01
    DATE,   //2010-01-01
    TIME,   //01:01:01
    ALL_SIMPLE, //20100101010101
    DATE_SIMPLE,//20100101
    TIME_SIMPLE,//010101
    DAY_SECS, //3600*24
    END,
};
class TimeDateX
{
public:
	TimeDateX(void);
	~TimeDateX(void);

    //min seconds
    static const int MINS = 60;
    //hour seconds
    static const int HOURS = 3600;
    //day seconds
    static const int DAYS = 86400;
    //week seconds
    static const int WEEKS = 604800;

public:
    std::string Str(ETimeStrType etype);
    //VST_ALLSIMPLE,VST_DATESIMPLE,VST_TIMESIMPLE,
    int64_t Number(ETimeStrType etype);

	void SetNow();
	void SetTime(const std::time_t &SurTime);
    std::time_t GetTime();

public:
    static std::string Str_s(ETimeStrType etype = ETimeStrType::ALL, time_t tmNow = 0);
    //VST_ALLSIMPLE,VST_DATESIMPLE,VST_TIMESIMPLE,
    static int64_t Number_s(ETimeStrType etype, time_t tmNow = 0);
	//2000-01-01 01:01:01
	static bool ConvTime_s(time_t& tmRet, const std::string& strAllTime);
    //2000-01-01
	static bool ConvDate_s(time_t& tmRet, const std::string& strDate, const char cSplit = '-');
    //01:01:01
    static uint32_t ConvTimeSec_s(const std::string& strTime);

private:
    std::time_t _timevalue = 0;
	std::tm     _tmstruct;
};
