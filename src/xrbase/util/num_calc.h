/*
 *
 *      Author: venture
 */
#pragma once

#include <stdint.h>
namespace num_calc
{
    template<typename T>
    T safe_add(T a, T b)
    {
        a += b;
        if (a < b)
            return T(-1);
        return a;
    }

    template<typename T>
    T safe_add(T a, T b, T max)
    {
        return min(safe_add(a, b), max);
    }

    template<typename T>
    T safe_add_place(T& a, T b)
    {
        return a = safe_add(a, b);
    }

    template<typename T>
    T safe_add_place(T& a, T b, T max)
    {
        return a = safe_add(a, b, max);
    }

    template<typename T>
    T safe_sub(T a, T b)
    {
        if (a >= b)
            return a - b;
        return 0;
    }

    template<typename T>
    T safe_sub(T a, T b, T min)
    {
        return ::max(safe_sub(a, b), min);
    }

    template<typename T>
    T safe_sub_place(T& a, T b)
    {
        return a = safe_sub(a, b);
    }

    template<typename T>
    T safe_sub_place(T& a, T b, T min)
    {
        return a = safe_sub(a, b, min);
    }
}

