#pragma once

#include <algorithm>
#include "log/LogMgr.h"
#include "container/fix_utility.h"

namespace fix
{
    template<class T, int N>
    struct fix_array
    {
        fix_array() :_elem_num(0) { }

        ~fix_array() { clear(); }

        T  elem[N] = {};
        typedef T* iterator;
        typedef const T* const_iterator;

        const int size() const
        {
            return _elem_num;
        }

        int add(const T& t)
        {
            ERROR_LOG_EXIT0(!this->full());
            elem[_elem_num++] = t;
            return 0;
Exit0:
            return -1;
        }

        const T* get(int idx) const
        {
            ERROR_LOG_EXIT0(idx >= 0 && idx < _elem_num);
            return &elem[idx];
Exit0:
            return nullptr;
        }

        T* mutable_add()
        {
            T* pRes = nullptr;
            ERROR_LOG_EXIT0(!this->full());
            pRes = new (&elem[_elem_num]) T();
            return &elem[_elem_num++];
Exit0:
            return nullptr;
        }

        int add_by_idx(const T& t, int idx)
        {
            ERROR_LOG_EXIT0(idx >= 0 && idx < N);
            ERROR_LOG_EXIT0(!this->full());
            for (int i = _elem_num; i > idx; --i)
            {
                elem[i] = elem[i - 1];
            }
            elem[idx] = t;
            _elem_num++;
            return 0;
Exit0:
            return -1;
        }

        int del(int idx)
        {
            ERROR_LOG_EXIT0(idx >= 0 && idx < _elem_num);
            for (int i = idx; i < _elem_num - 1; ++i)
            {
                elem[i] = elem[i + 1];
            }
            elem[_elem_num - 1].~T();
            --_elem_num;
            return 0;
Exit0:
            return -1;
        }

        int del_no_order(int idx)
        {
            ERROR_LOG_EXIT0(idx >= 0 && idx < _elem_num);
            elem[idx] = elem[_elem_num - 1];
            elem[_elem_num - 1].~T();
            --_elem_num;
            return 0;
Exit0:
            return -1;
        }

        template <typename KT>
        int find_idx(const KT& val) const
        {
            for (int i = 0; i < _elem_num; ++i)
            {
                if (elem[i] == val) return i;
            }
            return -1;
        }

        typedef bool (*FN_ARRAY_ELEM_EQUAL) (const T& lhs, const T& rhs);
        int find_idx(const T& val, FN_ARRAY_ELEM_EQUAL fn) const
        {
            for (int i = 0; i < _elem_num; ++i)
            {
                if (fn(elem[i], val)) return i;
            }
            return -1;
        }

        int capacity() const
        {
            return N;
        }

        bool full() const
        {
            return _elem_num >= N;
        }

        bool empty() const
        {
            return _elem_num == 0;
        }

        void resize(int sz)
        {
            ERROR_LOG_EXIT0(sz >= 0 && sz <= capacity());
            for (int i = sz; i < _elem_num; ++i)
            {
                elem[i].~T();
            }
            while (_elem_num < sz && mutable_add()) { }
            _elem_num = sz;
Exit0:
            return;
        }

        void clear()
        {
            for (int i = 0; i < _elem_num; ++i)
            {
                elem[i].~T();
            }
            _elem_num = 0;
        }

        //order interface
        inline void sort(void)
        {
            std::sort(elem, elem + _elem_num);
        }
        template <typename KT>
        int find_order(const KT& kval) const
        {
            const T* p_elem = std::lower_bound(elem, elem + _elem_num, kval);
            if (p_elem >= elem + _elem_num) return -1;
            return (*p_elem == kval) ? int(p_elem - elem) : -1;
        }
        int insert_order(const T& val)
        {
            if (full()) return -1;
            T* p_elem = std::upper_bound(elem, elem + _elem_num, val);
            for (T* p = elem + _elem_num; p > p_elem; --p)
            {
                *p = *(p - 1);
            }
            *p_elem = val;
            ++_elem_num;
            return int(p_elem - elem);
        }
        int insert_unique(const T& val)
        {
            int idx = find_order(val);
            if (idx >= 0)
            {
                elem[idx] = val;
                return idx;
            }
            else
            {
                return insert_order(val);
            }
        }
        int erase_order(const T& val)
        {
            int idx = find_order(val);
            if (idx >= 0)
            {
                del(idx);
            }
            return 0;
        }
        template <typename Compare>
        inline void sort(Compare cmp)
        {
            std::sort(elem, elem + _elem_num, cmp);
        }

        inline T& operator [](int idx)
        {
            return elem[idx];
        }

        inline const T& operator [](int idx) const
        {
            return elem[idx];
        }

        iterator begin() { return cast_ptr(T, elem); }
        iterator end() { return cast_ptr(T, elem) + _elem_num; }
        const_iterator begin() const { return reinterpret_cast<const T*>(elem); }
        const_iterator end()   const { return reinterpret_cast<const T*>(elem) + _elem_num; }

    private:
        int _elem_num = 0;
    };
}
