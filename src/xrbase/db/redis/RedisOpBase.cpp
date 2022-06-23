/*
 *
 *      Author: venture
 */
#include "pch.h"

#include "hiredis/hiredis.h"
#include "RedisOpBase.h"
#include <time.h>

#ifdef _MSC_VER
#include <WinSock2.h>
#endif // _MSC_VER
#include "time/time_func_x.h"

RedisOpBase::RedisOpBase()
{
	_timeouts = 300;
}

RedisOpBase::~RedisOpBase()
{
	if (_context != nullptr) redisFree(_context);
	_context = nullptr;
}

bool RedisOpBase::Init(const string& szIP, uint16_t wPort, const string& szPwd)
{
    _tmLast = get_curr_time();
	_szIP = szIP;
	_wPort = wPort;
	_szPwd = szPwd;

	return ReInit();
}

void RedisOpBase::UnInit()
{

}

bool RedisOpBase::ReInit()
{
	if(_context != nullptr) redisFree(_context);

    timeval tmv;
    tmv.tv_sec = 3;
    tmv.tv_usec = 0;
    _tmLast = get_curr_time();

    ++_loginTimes;
	_context = nullptr;
    _context = redisConnectWithTimeout(_szIP.c_str(), _wPort, tmv);
    if (_context == nullptr)
    {
        LOG_ERROR("Redis Connection error: context %s:%d, times:%u"
            , _szIP.c_str(), _wPort, _loginTimes);
        return false;
    }
    else if (_context->err)
    {
        LOG_ERROR("Redis Connection error: %s:%d, times:%u", _szIP.c_str(), _wPort, _loginTimes);
        redisFree(_context);
        _context = nullptr;
        return false;
    }

	//密码验证
	if (!_szPwd.empty())
	{
		redisReply* reply = (redisReply*)RunCommand("AUTH %s", _szPwd.c_str());
		if (reply == nullptr || reply->type != REDIS_REPLY_STATUS || strcmp(reply->str, "OK") != 0)
		{
			if (reply)
				freeReplyObject(reply);

            redisFree(_context);
            _context = nullptr;
			LOG_ERROR("Redis Connection user pwd auth error: %s:%d:%s, times:%u"
				, _szIP.c_str(), _wPort, _szPwd.c_str(), _loginTimes);
			return false;
		}
		freeReplyObject(reply);
	}

    LOG_CUSTOM("redis", "Redis Connection success: %s:%d, times:%u", _szIP.c_str(), _wPort, _loginTimes);
    return true;
}

void RedisOpBase::Close()
{
    if (_context == nullptr) return;

    redisFree(_context);
    _context = nullptr;
}

void RedisOpBase::MainLoop(time_t tmTick)
{
    if ((get_curr_time() - _tmLast) >= _timeouts)
    {
        Close();
    }
}

redisReply* RedisOpBase::RunAppendReply()
{
	if (_context == nullptr)
	{
		return nullptr;
	}

	void *reply = nullptr;
	if(redisGetReply(_context, &reply) != REDIS_OK)
	{
		freeReplyObject(reply);
		reply = nullptr;
	}

	return (redisReply*)reply;
}
