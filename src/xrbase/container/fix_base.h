#pragma once

#include "container/fix_utility.h"
#include <stdint.h>

namespace fix
{
    template<typename T>
    struct array_node
    {
        array_node() { }
        size_t used = false;
        char data[sizeof(T)] = {};
    };
}

template<typename T, size_t N>
struct fix_base
{
    static_assert(sizeof(T) >= sizeof(size_t), "type size not enough");

    typedef fix::array_node<T> node;
    static const size_t npos = static_cast<size_t>(-1);

    size_t used_count = 0;
    size_t free_head = 0;
    char memory[sizeof(node) * N] = {};

    fix_base() { }
    ~fix_base() { clear(); }

    fix_base(const fix_base& array) { __copy(array); }
    fix_base& operator=(const fix_base& array)
    {
        if (this == &array)
            return *this;

        clear();
        __copy(array);
        return *this;
    }

    void init()
    {
        used_count = 0;
        free_head = 0;
        __init();
    }

    void uninit() { clear(); }

    void __init()
    {
        // link the free slots
        for (size_t i = 0; i < N; ++i)
        {
            node* n = __at(i);
            new (n) node();
            n->used = false;
            *cast_ptr(size_t, n->data) = (i == N - 1) ? npos : i + 1;
        }
    }

    void __copy(const fix_base& array)
    {
        for (size_t i = 0; i < array.capacity(); ++i)
        {
            const node* on = array.__at(i); // old node
            node* nn = __at(i);             // new node

            if (on->used)
            {
                nn->used = true;
                const T* ot = cast_ptr(T, const_cast<char*>(on->data)); // old T
                T* nt = cast_ptr(T, nn->data);                          // new T
                new (nt) T(*ot);
            }
            else
            {
                nn->used = false;
                const size_t* oi = cast_ptr(size_t, const_cast<char*>(on->data)); // old index
                size_t* ni = cast_ptr(size_t, nn->data);                          // new index
                *ni = *oi;
            }
        }

        used_count = array.used_count;
        free_head = array.free_head;
    }

    node* __at(size_t index)
    {
        return cast_ptr(node, memory) + index;
    }

    const node* __at(size_t index) const
    {
        return cast_ptr(node, const_cast<char*>(memory)) + index;
    }

    void __add_free(size_t index)
    {
        node* n = __at(index);
        n->used = false;
        *cast_ptr(size_t, n->data) = free_head;

        free_head = index;
        --used_count;
    }

    void __add_used(size_t index)
    {
        __at(index)->used = true;
        ++used_count;
    }

    size_t size()     const { return used_count; }
    size_t capacity() const { return N; }

    bool full()  const
    {
        if (used_count >= N)
        {
            CHECK_LOG_ERROR(free_head == npos);
            return true;
        }
        return false;
    }

    bool empty() const
    {
        if (used_count <= 0)
        {
            CHECK_LOG_RETVAL(free_head != npos, false);
            return true;
        }
        return false;
    }

    void clear()
    {
        for (size_t i = 0; i < N; ++i)
        {
            node* n = __at(i);
            if (!n->used)
                continue;

            T* t = cast_ptr(T, n->data);
            t->~T();
            __add_free(i);
        }
    }

    T* at(size_t index)
    {
        CHECK_LOG_RETVAL(index < N, nullptr);

        node* n = __at(index);
        if (n->used)
            return cast_ptr(T, n->data);

        return nullptr;
    }

    const T* at(size_t index) const
    {
        CHECK_LOG_RETVAL(index < N, nullptr);

        const node* n = __at(index);
        if (n->used)
            return cast_ptr(T, const_cast<char*>(n->data));

        return nullptr;
    }

    void erase(size_t index)
    {
        CHECK_LOG_RETNONE(index < N);

        node* n = __at(index);
        if (!n->used)
            return;

        T* t = cast_ptr(T, n->data);
        t->~T();
        __add_free(index);
    }

    template<typename... Args>
    fix::fpair<T*, size_t> emplace(Args&&... args)
    {
        size_t _npos = npos;
        fix::fpair<T*, size_t> p(nullptr, _npos);

        if (full())
            return p;

        CHECK_LOG_RETVAL(free_head != npos, p);

        node* n = __at(free_head);
        p.second = free_head;
        __add_used(free_head);
        free_head = *cast_ptr(size_t, n->data);

        T* t = cast_ptr(T, n->data);
        new (t) T(std::forward<Args>(args)...);
        p.first = t;

        return p;
    }

    size_t index(const T* t) const
    {
        CHECK_LOG_RETVAL(t, npos);

        node* n = cast_ptr(node, char_ptr(const_cast<T*>(t)) - offsetof(node, data));
        CHECK_LOG_ERROR(n->used);

        size_t offset = char_ptr(n) - memory;
        //CHECK_LOG_ERROR(offset % sizeof(node) == 0);

        size_t index = offset / sizeof(node);
        CHECK_LOG_RETVAL(index < N, npos);

        return index;
    }
};