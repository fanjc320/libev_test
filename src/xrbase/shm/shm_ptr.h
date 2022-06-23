
#pragma once
#include "shm/shm_mgr.h"
#include <stdint.h>
#include <assert.h>

namespace shm_mgr
{
    template <class T>
    struct shm_ptr
    {
        shm_ptr() { }
        shm_ptr(uint64_t mid) : _mid(mid) { }

        T* get_ptr()
        {
            return mid2ptr();
        }
        const T* get_ptr() const
        {
            return mid2ptr();
        }
        T* operator->()
        {
            T* data = get_ptr();
            assert(data);
            return data;
        }
        const T* operator->() const
        {
            const T* data = get_ptr();
            assert(data);
            return data;
        }
        uint64_t get_mid() const
        {
            return _mid;
        }
        bool operator==(const shm_ptr<T>& ptr) const
        {
            return _mid == ptr._mid;
        }
        bool operator!=(const shm_ptr<T>& ptr) const
        {
            return !(*this == ptr);
        }

        operator bool() const
        {
            return !is_null();
        }
        bool operator!() const
        {
            return is_null();
        }

        bool is_null() const
        {
            return _mid == UINT64_MAX;
        }

    private:
        T* mid2ptr() const
        {
            SHM_MGR* shm_mgr = get_shm_mgr();
            if (is_null()) return nullptr;
            return (T*) mid2ptr(_mid);
        }

    private:
        uint64_t _mid = UINT64_MAX;
    };
}