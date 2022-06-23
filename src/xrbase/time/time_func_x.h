/*
 *
 *      Author: venture
 */
#pragma once
#include <math.h>
#include <assert.h>
#include <string>
#include <ctime>

//������Ϸ��ʼ�����ֵ
#ifdef _MSC_VER
int gettimeofday(struct timeval* tp, struct timezone* tzp);
#endif

extern bool set_start_time(const char* szTime);
extern int64_t get_start_time();
extern int64_t get_start_offset();

//��õ��ص�ϵͳʱ��
extern time_t get_curr_time();
extern time_t set_curr_time();

extern int64_t get_tick_count();
extern int64_t set_tick_count();

//��õ�ǰ����ʱ�䣬 ��������ʱ��(0ʱ��)
extern time_t get_gm_time(time_t nTime = get_curr_time());
//��ȡʱ���0��ʱ���
extern time_t get_zero_time(time_t nTime = get_curr_time());
//��ȡʱ���µĵ�һ�쿪ʼ
extern time_t get_zero_month(time_t nTime = get_curr_time());
//��ȡʱ���¸��µĵ�һ�쿪ʼ
extern time_t get_zero_next_month(time_t nTime = get_curr_time());
//ͬһ����
extern bool is_same_month(time_t nTime1, time_t nTime2);
//��õ�ǰʱ��͸�������ʱ���ʱ�����ӣ�
extern short get_local_zone_min();
//��õ�ǰʱ��͸�������ʱ���ʱ��Сʱ��
extern short get_local_zone_hour();
//��õ�ǰʱ��͸�������ʱ���ʱ���룩
extern int get_local_zone_sec();
//���ʱ��
extern short get_time_zone();
// ����ʱ��llTime1��llTime2�Ƿ�Ϊͬһ�죬nClockSeconds��ʾһ����nClockSeconds�뿪ʼ������0�㿪ʼ
extern bool is_same_day(time_t llTime1, time_t llTime2, int nClockSeconds = 0);
//��õ�ǰ����ʱ���Ѿ�������������nClockSeconds��ʾһ����nClockSeconds�뿪ʼ������0�㿪ʼ
extern int get_local_days(time_t llGmTime, int nClockSeconds = 0);
// ����ʱ��llTime1��llTime2�Ƿ�Ϊͬһ�ܣ�nClockSeconds��ʾһ����nClockSeconds�뿪ʼ������0�㿪ʼ
extern bool is_same_week(time_t llTime1, time_t llTime2, int nClockSeconds = 0);
//��õ�ǰ����ʱ���Ѿ�������������nClockSeconds��ʾһ����nClockSeconds�뿪ʼ������0�㿪ʼ
extern int get_local_weeks(time_t llGmTime, int nClockSeconds = 0);
//��õ�ǰ����ʱ���ܼ���nClockSeconds��ʾһ����nClockSeconds�뿪ʼ������0�㿪ʼ
extern int get_local_weekday(time_t llGmTime, int nClockSeconds = 0);
//��õ�ǰ����ʱ�䵱���ѹ�ȥ���룬nClockSeconds��ʾһ����nClockSeconds�뿪ʼ������0�㿪ʼ
extern int get_Local_day_pastsecond(time_t llGmTime, int nClockSeconds = 0);
//��õ�ǰ����ʱ���·ݵڼ���
extern int get_local_monthday(time_t llGmTime);
