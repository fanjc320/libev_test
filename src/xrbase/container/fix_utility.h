#pragma once
#include <stdint.h>

#define cast_ptr(type, ptr) static_cast<type*>(static_cast<void*>(ptr))
#define char_ptr(ptr)       cast_ptr(char, ptr)

namespace fix
{
    template<size_t N>
    struct size_type
    {
        typedef size_t _size_t;
    };

    template<>
    struct size_type<INT16_MAX>
    {
        typedef uint16_t _size_t;
    };

    template<>
    struct size_type<INT8_MAX>
    {
        typedef uint8_t _size_t;
    };

    template<size_t N>
    struct size_type_auto
    {
        enum { SIZE_BOUND = (N <= INT8_MAX) ? INT8_MAX : (N > INT16_MAX ? N : INT16_MAX) };
        typedef typename size_type<SIZE_BOUND>::_size_t index_t;
    };

    template <size_t N, int square>
    struct square_count
    {
        static const size_t RET = N * square_count<N, square - 1>::RET;
    };

    template <size_t N>
    struct square_count<N, 1>
    {
        static const size_t RET = N;
    };
    template <size_t N>
    struct square_count<N, 2>
    {
        static const size_t RET = N * N;
    };

    template <size_t N>
    struct square_count<N, 4>
    {
        static const size_t RET = N * N * N * N;
    };

    template <size_t N>
    struct square_count<N, 6>
    {
        static const size_t RET = N * N * N * N * N * N;
    };

    template <size_t N>
    struct square_count<N, 8>
    {
        static const size_t RET = N * N * N * N * N * N * N * N;
    };

    /*
    * like std::pair, used for map/rbtree to represent value type
    */
    template<typename T1, typename T2>
    struct fpair
    {
        typedef T1 first_type;
        typedef T2 second_type;

        T1 first;
        T2 second;

        fpair() : first(), second() {}
        fpair(const T1& first, const T2& second) : first(first), second(second) {}
    };

    template<typename P>
    struct select1st
    {
        typedef typename P::first_type result_type;

        result_type& operator()(P& p) const { return p.first; }
        const result_type& operator()(const P& p) const { return p.first; }
    };

    template<typename P>
    struct select2nd
    {
        typedef typename P::second_type result_type;

        result_type& operator()(P& p) const { return p.second; }
        const result_type& operator()(const P& p) const { return p.second; }
    };

    template<bool B, typename T, typename F>
    struct if_;

    template<typename T, typename F>
    struct if_<true, T, F>
    {
        typedef T type;
    };

    template<typename T, typename F>
    struct if_<false, T, F>
    {
        typedef F type;
    };
}
