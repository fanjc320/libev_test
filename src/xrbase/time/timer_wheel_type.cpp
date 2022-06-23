
#include "pch.h"
#include "time/timer_wheel_type.h"
#include "log/LogMgr.h"

namespace timer_wheel
{
    const char* g_timer_name[TIMER_TYPE_MAX + 1] = { 0 };
#define DECLARE_TIMER_NAME(type_name) g_timer_name[type_name] = #type_name
    const char* type2name(int timer_type)
    {
        if (timer_type < 0 || timer_type >= TIMER_TYPE_MAX)
            return "";

        const char* ret = g_timer_name[timer_type];
        if (ret == nullptr)return "";
        return ret;
    }

    void init_type_name()
    {
        DECLARE_TIMER_NAME(TIMER_TYPE_NULL);
        DECLARE_TIMER_NAME(TIMER_TYPE_TEST);
        DECLARE_TIMER_NAME(TIMER_TYPE_MAX);
    }
}
