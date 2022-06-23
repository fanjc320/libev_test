#include "pch.h"
#include <math.h>
#include <algorithm>
#include "math_func.h"
#include "log/LogMgr.h"
#include "math/math_basic.h"
#include <bit>
#include <stdint.h>
#ifdef _MSC_VER
#include <intrin.h>
#endif // _MSC_VER

uint32_t POPCNT32(uint32_t n)
{
    return std::popcount(n);
}

uint64_t POPCNT64(uint32_t n)
{
    return std::popcount(n);
}

#ifdef _MSC_VER
 unsigned long long RDTSC(void)
{
    return __rdtsc();
}

 void INT3(void)
{
    __debugbreak();
}

 int64_t bsf(uint64_t val)
{
    int32_t ret = -1;
    if (val != 0)
        _BitScanForward64((unsigned long*) &ret, val);

    return ret;
}

 int64_t bsr(uint64_t val)
{
    int32_t ret = -1;

    if (val != 0)
        _BitScanReverse64((unsigned long*) &ret, val);

    return ret;
}
#else
 unsigned long long RDTSC(void)
{
    unsigned long long int l, h;
    __asm__ volatile ("rdtsc" : "=a" (l), "=d" (h));

    return (h << 32) + l;
}

 void INT3(void)
{
    __asm("int3");
}

 int64_t bsf(uint64_t val)
{
    long ret = -1;
    if (val != 0)
        asm("bsfq %1, %0":"=r"(ret) : "r"(val));

    return ret;
}

 int64_t bsr(uint64_t val)
{
    long ret = -1;
    if (val != 0)
        asm("bsrq %1, %0":"=r"(ret) : "r"(val));

    return ret;
}

#endif	// _MSC_VER

uint64_t round_up_2power(uint64_t val)
{
    return 1ULL << (bsr(val - 1) + 1);
}

uint64_t round_down_2power(uint64_t val)
{
    return 1ULL << (bsr(val));
}

bool is_2power(uint64_t val)
{
    return (val & (val - 1)) == 0;
}

void* move_ptr_align8(void* ptr, uint64_t offset)
{
    return (void*) ((((uint64_t) ptr + offset) + 7) & (~7));
}

void* move_ptr_align16(void* ptr, uint64_t offset)
{
    return (void*) ((((uint64_t) ptr + offset) + 15) & (~15));
}

void* move_ptr_align64(void* ptr, uint64_t offset)
{
    return (void*) ((((uint64_t) ptr + offset) + 63) & (~63));
}

void* move_ptr_align128(void* ptr, uint64_t offset)
{
    return (void*) ((((uint64_t) ptr + offset) + 127) & (~127));
}

void* move_ptr_roundup(void* ptr, uint64_t offset, uint64_t align)
{
    return (void*) ((((uint64_t) ptr + offset) + (align - 1)) & (~(align - 1)));
}

int rand_x(int range_max)
{
    if (range_max == 0) return 0;
    if (range_max > RAND_MAX)
        return (int) ((double(rand()) / double(RAND_MAX)) * (range_max));
    else
        return rand() % range_max;
}

//生成一个range_min~range_max-1的无符号整数
int rand_x(int range_min, int range_max)
{
    int rate = 0;
    if (range_min == range_max)
    {
        rate = range_min;
    }
    else
    {
        int r = range_max - range_min;
        if (r > RAND_MAX)
            rate = (int) ((double(rand()) / double(RAND_MAX)) * (r) +range_min);
        else
            rate = range_min + (rand() % r);
    }
    return rate;
}

double randf_x(double range_min, double range_max)
{
    return ((double(rand()) / double(RAND_MAX)) * (range_max - range_min)) + range_min;
}

void srand_x(uint32_t seed)
{
    srand(seed);
}

uint32_t g_dwRandSeed = 0;

void srand_ex(uint32_t dwSeed)
{
    g_dwRandSeed = dwSeed;
}

uint32_t get_srand_ex()
{
    return g_dwRandSeed;
}

int64_t rand_ex(int64_t llRange)
{
    if (llRange > 0)
    {
        g_dwRandSeed = (uint32_t) (g_dwRandSeed * 0x08088405) + 1;
        return (int64_t) ((g_dwRandSeed * llRange) >> 32);
    }
    else
        return 0;
}

int64_t rand_ex(int64_t llMin, int64_t llMax)
{
    return rand_ex(llMax - llMin) + llMin;
}

int32_t rand_from_arr(const int64_t pRange[], int32_t nRangeCount)
{
    int64_t nRangeTotal = 0;
    int64_t nCurRange = 0;
    ERROR_LOG_EXIT0(pRange);
    for (int32_t i = 0; i < nRangeCount; i++)
    {
        nRangeTotal += pRange[i];
    }
    ERROR_LOG_EXIT0(nRangeTotal > 0);
    nCurRange = rand_ex(nRangeTotal);

    for (int32_t i = 0; i < nRangeCount; i++)
    {
        if (nCurRange <= pRange[i])
        {
            return i;
        }
        nCurRange -= pRange[i];
    }

Exit0:
    return -1;
}

int32_t rand_from_arr_direct(const int64_t pRange[], int32_t nRangeCount)
{
    int64_t nRangeTotal = 0;
    int64_t nCurRange = 0;
    const int64_t* pRet = nullptr;
    ERROR_LOG_EXIT0(pRange && nRangeCount > 0);
    if (nRangeCount == 1) return 0;
    nRangeTotal = *(pRange + nRangeCount - 1);

    ERROR_LOG_EXIT0(nRangeTotal > 0);
    nCurRange = rand_ex(nRangeTotal);
    pRet = std::upper_bound(pRange, pRange + nRangeCount, nCurRange);
    ERROR_LOG_EXIT0(pRet != nullptr);
    return (int32_t) (pRet - pRange);
Exit0:
    return -1;
}

double round_x(double dVal, int iPlaces)
{
    double dRetval;
    double dMod = 0.0000001;
    if (dVal < 0.0) dMod = -0.0000001;
    dRetval = dVal;
    dRetval += (5.0 / pow(10.0, iPlaces + 1.0));
    dRetval *= pow(10.0, iPlaces);
    dRetval = floor(dRetval + dMod);
    dRetval /= pow(10.0, iPlaces);
    return dRetval;
}

uint64_t randu64_x()
{
    return (uint64_t) (((uint64_t) rand() << 32) | rand());
}

int64_t rands64_x()
{
    return (int64_t) (((int64_t) rand() << 32) | rand());
}

void rand_shuffle(int32_t arr[], int32_t len)
{
    int32_t m = len / 2;
    for (int32_t i = m; i < len; ++i)
    {
        int32_t k = rand() % (i + 1);
        if (k < m)
            std::swap(arr[k], arr[i]);
    }
}

int64_t get_abs(int64_t x)
{
    int64_t y = x >> 63;
    return ((x ^ y) - y);
}

int64_t num_sign(int64_t x)
{
    return (x >> 63) | (~((~x + 1) >> 63) + 1);
}


uint64_t round_up(uint64_t qwValue, uint64_t qwSize)
{
    return ((qwValue + qwSize - 1) & ~(qwSize - 1));
}

uint64_t round_down(uint64_t qwValue, uint64_t qwSize)
{
    return ((qwValue) & ~(qwSize - 1));
}

uint64_t roundup_order(uint64_t val, uint64_t alignment_order)
{
    return round_up(val, (1ull << alignment_order));
}

int64_t distance2d(int64_t x1, int64_t y1, int64_t x2, int64_t y2)
{
    return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

int64_t distance3d(int64_t x1, int64_t y1, int64_t z1, int64_t x2, int64_t y2, int64_t z2)
{
    return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1);
}


float InvSqrtf(float v)
{
    float x = v;
    float xhalf = 0.5f * x;
    int i = *(int*) &x;
    i = 0x5f3759df - (i >> 1); // 计算第一个近似根
    x = *(float*) &i;
    x = x * (1.5f - xhalf * x * x); // 牛顿迭代法
    return x;
}

double InvSqrt(double sq)
{
    double y = sq;
    double x2 = y * 0.5;
    int64_t i = *(int64_t*) &y;
    // The magic number is for doubles is from https://cs.uwaterloo.ca/~m32rober/rsqrt.pdf 
    i = 0x5fe6eb50c7b537a9 - (i >> 1);
    y = *(double*) &i;
    y = y * (1.5 - (x2 * y * y)); // 1st iteration 
    //  y = y * (1.5 - (x2 * y * y)); // 2nd iteration, this can be removed 
    return y;
}

int64_t quick_sqrt(int64_t sq)
{
    return (int64_t) (1.0 / InvSqrt((double) sq));
}

float quick_sqrtf(float sq)
{
    return 1.0f / InvSqrtf(sq);
}

double quick_sqrtlf(double sq)
{
    return 1.0 / InvSqrt(sq);
}

int32_t round_div_t(int32_t dividend, int32_t divisor)
{
    int32_t quotient = dividend / divisor;
    int32_t remainder = dividend % divisor;
    if (remainder * 2 < divisor)
        return quotient;
    else if (remainder * 2 > divisor)
        return quotient + 1;
    else
        return (quotient + 1) & (~1);
}

size_t hash_bytes(const void* buf, size_t len)
{
    size_t h = 0;
    for (size_t i = 0; i < len - 1; ++i)
    {
        h = ((char*)buf)[i] + ((h << 5) - h);
    }
    return h;
}

size_t hash_bytes_fnv(const void* buf, size_t len)
{
    std::hash<std::string_view> hash1;
    return hash1(std::string_view((char*)buf, len));
}
