#pragma once
#include <stdint.h>

enum E_TIMER_TYPE
{
    TIMER_TYPE_NULL = 0,
    TIMER_TYPE_TEST = 1,
    TIMER_TYPE_MAX = (1 << 10) - 1,
};

namespace timer_wheel
{
    extern void init_type_name();
    extern const char* type2name(int timer_type);
}
