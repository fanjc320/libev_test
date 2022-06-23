/*
 *
 *      Author: venture
 */
#pragma once
#include <math.h>
#include <assert.h>
#include <string>
#include <ctime>

//设置游戏开始的误差值
#ifdef _MSC_VER
int gettimeofday(struct timeval* tp, struct timezone* tzp);
#endif

extern bool set_start_time(const char* szTime);
extern int64_t get_start_time();
extern int64_t get_start_offset();

//获得当地的系统时间
extern time_t get_curr_time();
extern time_t set_curr_time();

extern int64_t get_tick_count();
extern int64_t set_tick_count();

//获得当前世界时间， 格林威治时间(0时区)
extern time_t get_gm_time(time_t nTime = get_curr_time());
//获取时间的0点时间戳
extern time_t get_zero_time(time_t nTime = get_curr_time());
//获取时间月的第一天开始
extern time_t get_zero_month(time_t nTime = get_curr_time());
//获取时间下个月的第一天开始
extern time_t get_zero_next_month(time_t nTime = get_curr_time());
//同一个月
extern bool is_same_month(time_t nTime1, time_t nTime2);
//获得当前时间和格林威治时间的时间差（分钟）
extern short get_local_zone_min();
//获得当前时间和格林威治时间的时间差（小时）
extern short get_local_zone_hour();
//获得当前时间和格林威治时间的时间差（秒）
extern int get_local_zone_sec();
//获得时区
extern short get_time_zone();
// 计算时间llTime1和llTime2是否为同一天，nClockSeconds表示一天以nClockSeconds秒开始，而非0点开始
extern bool is_same_day(time_t llTime1, time_t llTime2, int nClockSeconds = 0);
//获得当前世界时间已经历过的天数，nClockSeconds表示一天以nClockSeconds秒开始，而非0点开始
extern int get_local_days(time_t llGmTime, int nClockSeconds = 0);
// 计算时间llTime1和llTime2是否为同一周，nClockSeconds表示一天以nClockSeconds秒开始，而非0点开始
extern bool is_same_week(time_t llTime1, time_t llTime2, int nClockSeconds = 0);
//获得当前世界时间已经历过的周数，nClockSeconds表示一天以nClockSeconds秒开始，而非0点开始
extern int get_local_weeks(time_t llGmTime, int nClockSeconds = 0);
//获得当前世界时间周几，nClockSeconds表示一天以nClockSeconds秒开始，而非0点开始
extern int get_local_weekday(time_t llGmTime, int nClockSeconds = 0);
//获得当前世界时间当日已过去的秒，nClockSeconds表示一天以nClockSeconds秒开始，而非0点开始
extern int get_Local_day_pastsecond(time_t llGmTime, int nClockSeconds = 0);
//获得当前世界时间月份第几天
extern int get_local_monthday(time_t llGmTime);
