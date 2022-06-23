#pragma once

#include <math.h>
#include <algorithm>
#include <stdint.h>
#include <stdlib.h>

#ifdef _MSC_VER
#define likely(x)       (x)
#define unlikely(x)     (x)
#else
#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)
#endif // _MSC_VER

const int32_t    MEM_SIZE_256M = 268435456; //256*1024*1024
const int32_t    MEM_SIZE_512M = 536870912; //512*1024*1024
const int32_t    MEM_SIZE_1M = 1048576; //1024*1024
const int32_t    MEM_SIZE_256K = 262144;  //1024*256
const int32_t    MEM_SIZE_128K = 131072;  //128*1024
const int32_t    MEM_SIZE_64K = 65536;  //64*1024
const int32_t    MEM_SIZE_32K = 32768;  //32*1024
const int32_t    MEM_SIZE_16K = 1024;  //1024*16
const int32_t    MEM_SIZE_4K = 1024;  //1024*4
const int32_t    MEM_SIZE_1K = 1024;  //1024

#define MATH_SWAP64(x)                                          \
    ((((x) & (unsigned long long)0xff00000000000000ull) >> 56)   \
     | (((x) & (unsigned long long)0x00ff000000000000ull) >> 40) \
     | (((x) & (unsigned long long)0x0000ff0000000000ull) >> 24) \
     | (((x) & (unsigned long long)0x000000ff00000000ull) >> 8) \
     | (((x) & (unsigned long long)0x00000000ff000000) << 8)    \
     | (((x) & (unsigned long long)0x0000000000ff0000) << 24)   \
     | (((x) & (unsigned long long)0x000000000000ff00) << 40)   \
     | (((x) & (unsigned long long)0x00000000000000ff) << 56))

#define MATH_SWAP32(x) \
    ((((x)&0xff000000) >> 24) | (((x)&0x00ff0000) >> 8) | (((x)&0x0000ff00) << 8) | (((x)&0x000000ff) << 24))

#define MATH_SWAP16(x) ((((x)&0xff00) >> 8) | (((x)&0x00ff) << 8))

static const double PI = acos(-1.0);
#define TWOPI (2.0f * PI)

#ifndef INFINITY
const double INFINITY = 1e20; //无穷大
#endif

#define ALIGN8(x) (((x) + 7LL) & (~7LL))
#define FEQ(a, b) (fabs((a) - (b)) < (1e-5))

#define MATH_SET_BIT(val, n) (val |= 1 << n)
#define MATH_SET_BIT_U64(val, n) (val |= 1ull << n)

#define MATH_CLEAR_BIT(val, n) (val &= ~(1 << n))
#define MATH_CLEAR_BIT_U64(val, n) (val &= ~(1ull << n))

#define MATH_IS_SET(val, n) (!!((val) & (1 << (n))))
#define MATH_IS_SET_U64(val, n) (!!((val) & (1ull << (n))))

#define FLOAT_TO_INT(x) (*((int *)&(x))) //整数部分
#define FLOAT_GET_EXP(x) ((FLOAT_TO_INT(x) & 0x7f800000) >> 23) //进制部分
#define FLOAT_GET_SIG(x) ((FLOAT_TO_INT(x)) & 0x001fffff) //小数部分

#define FLOAT_IS_INF(x) ((FLOAT_GET_EXP(x) == 0xff) && (FLOAT_GET_SIG(x) == 0x0)) //无穷大
#define FLOAT_IS_NAN(x) ((FLOAT_GET_EXP(x) == 0xff) && (FLOAT_GET_SIG(x) != 0x0)) //非数字

#define MAKE_INT64(hi, lo) ((((int64_t)(hi)) << 32) | (int64_t)(lo))
#define MAKE_UINT64(hi, lo) ((((uint64_t)(hi)) << 32) | (uint64_t)(lo))
#define MAKE_INT32(hi, lo) ((((int32_t)(hi)) << 16) | (int32_t)(lo))
#define MAKE_UINT32(hi, lo) ((((uint32_t)(hi)) << 16) | (uint32_t)(lo))
#define MAkE_UINT16(hi, lo) ((((uint16_t)(hi)) << 8) | (uint16_t)(lo))

#define FILT_HIGH32(id) ((id) >> 32)
#define FILT_LOW32(id) (0x00000000FFFFFFFF & (id))
#define FILT_HIGH16(id) ((id) >> 16)
#define FILT_LOW16(id) (0x0000FFFF&(id))
#define FILT_HIGH8(id) ((id) >> 8)
#define FILT_LOW8(id) (0x00FF&(id))

#define MK_MOD_INT(hi, lo, n)((hi)*(int64)pow(10, (n))+(lo))
#define FLT_MOD_HIGH(id, n) ((id)/(int64)pow(10, (n)))
#define FLT_MOD_LOW(id, n) ((id)%(int64)pow(10, (n)))

// 换算角度值
#define ANGLE_TO_RADIAN(x) (float(x) * TWOPI / 360.0f)
