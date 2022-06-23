#pragma once

#include <stdint.h>
#include <limits.h>

namespace fix
{
    template<uint32_t N>
    struct fix_bit_list
    {
    public:
        fix_bit_list()
        {
            static_assert(N > 0, "Count must > 0");
        }

        void init()
        {
            memset(_BitFlag, 0, sizeof(_BitFlag));
        }

        void clear()
        {
            memset(_BitFlag, 0, sizeof(_BitFlag));
        }

        uint32_t set_num() const
        {
            uint32_t c = 0;
            for (uint32_t i = 0; i < ARR_LEN(_BitFlag); ++i)
                c += POPCNT64(_BitFlag[i]);
            return c;
        }

        bool empty() const
        {
            return set_num() == 0;
        }

        void set(uint32_t dwID)
        {
            if (dwID < N)
            {
                uint32_t dwIndex = dwID / 64;
                uint32_t dwOffset = dwID % 64;
                _BitFlag[dwIndex] |= (1ULL << (uint64_t) dwOffset);
            }
        }

        void unset(uint32_t dwID)
        {
            if (dwID < N)
            {
                uint32_t dwIndex = dwID / 64;
                uint32_t dwOffset = dwID % 64;
                _BitFlag[dwIndex] &= ~(1ULL << (uint64_t) dwOffset);
            }
        }
        bool check(uint32_t dwID) const
        {
            if (dwID < N)
            {
                uint32_t dwIndex = dwID / 64;
                uint32_t dwOffset = dwID % 64;
                return (_BitFlag[dwIndex] & (1ULL << (uint64_t)dwOffset)) != 0 ? true : false;
            }
            else
            {
                return false;
            }
        }

    protected:
        uint64_t _BitFlag[(N-1 / 64) + 1] = {};
    };

    template <typename _T>
    struct fix_bit
    {
    public:
#pragma warning(push)
#pragma warning(disable : 4296) // expression is always true (/Wall)
        using T = std::conditional_t<sizeof(_T) <= sizeof(unsigned long) * 8, unsigned long, unsigned long long>;
#pragma warning(pop)

        fix_bit(void) 
        {
            static_assert(std::is_integral<_T>::value, "Type V must be intergral!");
        }

        bool check(T eBit) const
        {
            return (_FlagNum & (1 << eBit)) > 0;
        }

        void set(T eBit)
        {
            _FlagNum |= (1 << eBit);
        }

        void unset(T eBit)
        {
            _FlagNum &= ~(1 << eBit);
        }

        uint32_t set_num() const
        {
            return POPCNT64(_FlagNum);
        }

        bool empty() const
        {
            return set_num() == 0;
        }

        void clear(void)
        {
            _FlagNum = 0;
        }

        bool inbits(T dwMask) const
        {
            return (_FlagNum > 0 && (_FlagNum & dwMask) == dwMask) || (_FlagNum == 0 && dwMask == 0);
        }

        bool no_inbits(T dwMask) const
        {
            return (_FlagNum & dwMask) == 0;
        }

        T get(void)
        {
            return _FlagNum;
        }

    private:
        T _FlagNum = 0;
    };
};