/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <time.h>
#include <ctime>
#include <chrono>
#include <sys/timeb.h>
#include "string/str_format.h"
#include "time_func_x.h"
#include "TimeDateX.h"
#include "time/TimeCount.h"

#ifdef _MSC_VER
#include <windows.h>
#include <stdint.h>  // portable: uint64_t   MSVC: __int64
#include <WinSock2.h>
#endif // _MSC_VER

const uint32_t	DaySeconds = 86400;
const uint32_t	WeekSeconds = 604800;
	
static time_t  g_start_time = 0;
static int64_t g_start_offset = 0;
static int64_t g_tick_count = 0;
static time_t  g_tmCurr = 0;

#ifdef _MSC_VER
int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct
    // epoch has 9 trailing zero's This magic number is the number of 100
    // nanosecond intervals since January 1, 1601 (UTC) until 00:00:00
    // January 1, 1970
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME system_time;
    FILETIME file_time;
    uint64_t time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time = ((uint64_t) file_time.dwLowDateTime);
    time += ((uint64_t) file_time.dwHighDateTime) << 32;

    tp->tv_sec = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}

#endif // _MSC_VER



bool set_start_time(const char* szTime)
{
	bool ret = TimeDateX::ConvTime_s(g_start_time, szTime);
	if (!ret) return false;
	g_start_offset = time(nullptr) - g_start_time;
	return true;
}

time_t get_start_time()
{
	return g_start_time;
}

int64_t get_start_offset()
{
	return g_start_offset;
}

bool is_same_day(time_t llTime1, time_t llTime2, int nClockSeconds/* = 0*/)
{
	return (get_local_days(llTime1, nClockSeconds) == get_local_days(llTime2, nClockSeconds));
}

int get_local_days(time_t llGmTime, int nClockSeconds/* = 0*/)
{
	int64_t ret = (llGmTime - get_local_zone_sec() - nClockSeconds) / (DaySeconds);
	return (int)ret;
}

bool is_same_week(time_t llTime1, time_t llTime2, int nClockSeconds /*= 0*/)
{
	return (get_local_weeks(llTime1, nClockSeconds) == get_local_weeks(llTime2, nClockSeconds));
}

int get_local_weeks(time_t llGmTime, int nClockSeconds/* = 0*/)
{
	int64_t ret = (llGmTime - get_local_zone_sec() - 4 * DaySeconds - nClockSeconds) / (WeekSeconds);
	return (int)ret;
}

int get_local_weekday(time_t llGmTime, int nClockSeconds/* = 0*/)
{
	int nLeftWeekSeconds = (llGmTime - get_local_zone_sec() - 4 * DaySeconds - nClockSeconds) % (WeekSeconds);
	return nLeftWeekSeconds / DaySeconds + 1;
}

int get_Local_day_pastsecond(time_t llGmTime, int nClockSeconds /*= 0*/)
{
	return (llGmTime - get_local_zone_sec() - nClockSeconds) % (DaySeconds);
}

 short get_time_zone()
{
	short timezone = get_local_zone_hour();
	return -timezone;
}

 int64_t _get_tick_count()
 {
     auto time_now = std::chrono::system_clock::now();
     auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());
     return duration_in_ms.count();
 }

//毫秒
int64_t get_tick_count()
{
    if (g_tick_count == 0)
        return _get_tick_count();
#ifdef _MSC_VER
    return _get_tick_count();
#endif // _MSC_VER
    return g_tick_count;
}

 time_t get_curr_time()
{
	if (g_tmCurr == 0)
		return std::time(nullptr);
#ifdef _MSC_VER
	return std::time(nullptr);
#endif // _MSC_VER
	return g_tmCurr;
}

time_t set_curr_time()
{
	g_tmCurr = std::time(nullptr);
	return g_tmCurr;
}

time_t set_tick_count()
{
    g_tick_count = _get_tick_count();
    return g_tick_count;
}

time_t get_gm_time(time_t nTime)
{
	struct tm struct_tm;
	gmtime_s(&struct_tm, &nTime);
	return mktime(&struct_tm);
}

//获取当地时间当天0点时间戳
time_t get_zero_time(time_t nTime)
{
	tm tmZero;
	localtime_s(&tmZero, &nTime);
	tmZero.tm_hour = 0;
	tmZero.tm_min = 0;
	tmZero.tm_sec = 0;
	return mktime(&tmZero);
}

time_t get_zero_month(time_t nTime)
{
	tm tmMon;
	localtime_s(&tmMon, &nTime);
	tmMon.tm_mday = 1;
	tmMon.tm_hour = 0;
	tmMon.tm_min = 0;
	tmMon.tm_sec = 0;
	return mktime(&tmMon);
}

time_t get_zero_next_month(time_t nTime)
{
	struct tm tmMon;
	localtime_s(&tmMon, &nTime);
	if (tmMon.tm_mon >= 11) //月份为0-11
	{
		tmMon.tm_year += 1;
		tmMon.tm_mon = 0;
	}
	else
		tmMon.tm_mon += 1;

	tmMon.tm_mday = 1;
	tmMon.tm_hour = 0;
	tmMon.tm_min = 0;
	tmMon.tm_sec = 0;
	return mktime(&tmMon);
}

bool is_same_month(time_t nTime1, time_t nTime2)
{
	tm tmMon1;
	tm tmMon2;
	localtime_s(&tmMon1, &nTime1);
	localtime_s(&tmMon2, &nTime2);

	if (tmMon1.tm_year != tmMon2.tm_year)
		return false;

	return tmMon1.tm_mon == tmMon2.tm_mon;
}

short get_local_zone_min()
{
	static short shZone = 0;
	if (shZone) return shZone;
	struct timeb tp;
	ftime(&tp);
	shZone = tp.timezone;
	return shZone;
}

short get_local_zone_hour()
{
	static short shZone = 0;
	if (shZone) return shZone;
	shZone = get_local_zone_min() / 60;
	return shZone;
}

int get_local_zone_sec()
{
	static int nZone = 0;
	if (nZone) return nZone;
	nZone = get_local_zone_min() * 60;
	return nZone;
}

int get_local_monthday(time_t llGmTime)
{
	tm tmMon;
	localtime_s(&tmMon, &llGmTime);
	return tmMon.tm_mday;
}
