#pragma once
#include <stdint.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include "math/math_basic.h"

extern unsigned long long RDTSC(void);
extern void INT3(void);

extern uint32_t POPCNT32(uint32_t n);
extern uint64_t POPCNT64(uint32_t n);
extern int64_t bsf(uint64_t val);
extern int64_t bsr(uint64_t val);

extern uint64_t round_up_2power(uint64_t val);
extern uint64_t round_down_2power(uint64_t val) ;
extern bool  is_2power(uint64_t val);
extern void* move_ptr_align8(void* ptr, uint64_t offset);
extern void* move_ptr_align16(void* ptr, uint64_t offset);
extern void* move_ptr_align64(void* ptr, uint64_t offset);
extern void* move_ptr_align128(void* ptr, uint64_t offset);
extern void* move_ptr_roundup(void* ptr, uint64_t offset, uint64_t align);

//最小值~~最大值
//由于windows下rand的数值范围为0-7fff(32767),所以添加此函数
extern int      rand_x(int range_max);
extern int      rand_x(int range_min, int range_max);
extern double   randf_x(double range_min, double range_max);
extern void     srand_x(uint32_t seed);
extern void     rand_shuffle(int32_t arr[], int32_t len);
extern uint64_t randu64_x();
extern int64_t  rands64_x();

//自己定义的随机函数
extern void     srand_ex(uint32_t dwSeed);
extern int64_t  rand_ex(int64_t llRange);
extern int64_t  rand_ex(int64_t llMin, int64_t llMax);
extern uint32_t get_srand_ex();

extern int32_t rand_from_arr(const int64_t pRange[], int32_t nRangeCount);
extern int32_t rand_from_arr_direct(const int64_t pRange[], int32_t nRangeCount);

//浮点数截取位数
extern double   round_x(double val, int places);
extern uint64_t round_up(uint64_t qwValue, uint64_t qwSize = 8LL);
extern uint64_t round_down(uint64_t qwValue, uint64_t qwSize = 8LL);
extern uint64_t roundup_order(uint64_t val, uint64_t alignment_order);

template< typename T>
extern T  min_x(T a, T b)
{
    return a >= b ? b : a;
}

template< typename T>
extern T  max_x(T a, T b)
{
    return a >= b ? a : b;
}

extern int64_t  get_abs(int64_t x);
extern int64_t  num_sign(int64_t x);

extern int64_t  distance2d(int64_t x1, int64_t y1, int64_t x2, int64_t y2);
extern int64_t  distance3d(int64_t x1, int64_t y1, int64_t z1, int64_t x2, int64_t y2, int64_t z2);

extern float    InvSqrtf(float v);
extern double   InvSqrt(double sq);
extern int64_t  quick_sqrt(int64_t sq);
extern float    quick_sqrtf(float sq);
extern double   quick_sqrtlf(double sq);
extern int32_t  round_div_t(int32_t dividend, int32_t divisor);

template <typename T>
extern void swap_x(T& a, T& b)
{
    T tmp = a;
    a = b;
    b = tmp;
}

// 从数组中选出不重复的N个元素
template <typename T>
extern int rand_array_unique(const T arr[], int arr_num, T rand_arr[], int& rand_arr_num)
{
    if (rand_arr_num >= arr_num)
    {
        rand_arr_num = arr_num;
        memcpy(rand_arr, arr, sizeof(T) * arr_num);
    }
    else if (rand_arr_num == 1)
    {
        int idx = rand_x(0, arr_num - 1);
        *rand_arr = arr[idx];
    }
    else
    {
        T tmp_arr[arr_num];
        int tmp_arr_num = arr_num;
        memcpy(tmp_arr, arr, sizeof(T) * arr_num);

        for (int i = 0; i < rand_arr_num; ++i)
        {
            int idx = rand_x(0, tmp_arr_num - 1);
            rand_arr[i] = tmp_arr[idx];
            tmp_arr[idx] = tmp_arr[tmp_arr_num - 1];
            --tmp_arr_num;
        }
    }

    return rand_arr_num;
}

//实现vector的唯一去重处理
template <typename T>
extern void unique_vector(std::vector<T>& vec)
{
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

//实现vector的唯一去重处理
//返回剩余数组的大小
template <typename T>
extern size_t unique_vector(T lst[], size_t size)
{
    std::sort(lst, lst + size);
    return std::unique(lst, lst + size) - lst;
}

extern size_t hash_bytes(const void* buf, size_t len);
extern size_t hash_bytes_fnv(const void* buf, size_t len);

template <typename T>
inline size_t hash_bytes(T val)
{
    if (std::is_integral<T>::value) return val;
    return hash_bytes(&val, sizeof(val));
}
