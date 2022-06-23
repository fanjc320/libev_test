/*
 *
 *      Author: venture
 */
#include "pch.h"

#ifdef _MSC_VER
#include <direct.h>
#include <io.h>
#else
#include <stdarg.h>
#include <sys/stat.h>
#endif

#include "util/func_util.h"
#include "util/md5.h"
#include "log/LogMgr.h"
#include <sys/timeb.h>
#include "text/char_set.h"
#include "time/time_func_x.h"

static uint32_t g_id = 0;

union U_GEN_ID
{
    struct
    {
        int64_t tick : 48;
        int64_t cnt : 16;
    };
    int64_t id;
};

int64_t gen_id()
{   
    U_GEN_ID id;
    id.tick = get_tick_count();
    id.cnt = ++g_id;
    return id.id;
}

//////////计算md5值
std::string make_md5(const std::string &str)
{
    MD5 md5;
    md5.update(str);
    return md5.toString();
}

std::string make_md5(const char* str, size_t len)
{
    MD5 md5;
    md5.update(str, len);
    return md5.toString();
}

std::string make_md5_16(const std::string &str)
{
    std::string ret = make_md5(str);
	ret = ret.substr(8, 16);
	return ret;
}

std::string make_md5_16(const char* str, size_t len)
{
    std::string ret = make_md5(str, len);
    ret = ret.substr(8, 16);
    return ret;
}
