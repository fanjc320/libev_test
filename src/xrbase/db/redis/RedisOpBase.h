/*
 *
 *      Author: venture
 */

#pragma once

#include "dtype.h"
#include "hiredis/hiredis.h"
#include <string>
#include <vector>
#include <algorithm>
#include "log/LogMgr.h"
#include <stdint.h>
#include "time/time_func_x.h"

using namespace std;

class RedisOpBase
{
public:	
	RedisOpBase();
    RedisOpBase(uint32_t timeout) { _timeouts = timeout; }
	virtual ~RedisOpBase();

	bool Init(const string& szIP, uint16_t wPort, const string& szPwd);
	void UnInit();

	bool ReInit();
    void Close();

    virtual void MainLoop(time_t tmTick);

protected:
	template <typename... Args>
	redisReply *RunCommand(const char *format, Args&&... args);

	template <typename... Args>
	bool AppendCommand(const char* format, Args&&... args);

	redisReply* RunAppendReply();

	string _szIP = "";
	uint16_t _wPort = 0;
	string _szPwd = "";
    uint32_t _loginTimes = 0;
    uint32_t _timeouts = 300;

    time_t  _tmLast = get_curr_time();
	redisContext* _context = nullptr;
};

template <typename... Args>
redisReply* RedisOpBase::RunCommand(const char* format, Args&&... args)
{
    _tmLast = get_curr_time();
	redisReply *reply = nullptr;
	for (uint32_t i = 0; i < 60; ++i)
	{
        if (_context == nullptr && !ReInit())
        {
            this_thread::sleep_for(chrono::milliseconds(1000));
            continue;
        }

		reply = (redisReply*) redisCommand(_context, format, std::forward<Args>(args)...);
		if (nullptr == reply)
		{
			ReInit();
		}
		else if(reply->type == REDIS_REPLY_ERROR)
		{
			LOG_ERROR("Redis RunCommand error: %s" , reply->str);
			freeReplyObject(reply);
			reply = nullptr;
		}
		else
		{
			break;
		}
	}

	return reply;
}

template <typename... Args>
bool RedisOpBase::AppendCommand(const char* format, Args&&... args)
{
    _tmLast = get_curr_time();
	int32_t res = -1;
	for (uint32_t i = 0; i < 60; ++i)
	{
        if (_context == nullptr && !ReInit())
        {
            this_thread::sleep_for(chrono::milliseconds(1000));
            continue;
        }

		res = redisAppendCommand(_context, format, std::forward<Args>(args)...);
		if (res == REDIS_ERR)
		{
			ReInit();
		}
		else
		{
			break;
		}
	}

	if(res != REDIS_OK)
	{
		ReInit();
		return false;
	}

	return true;
}