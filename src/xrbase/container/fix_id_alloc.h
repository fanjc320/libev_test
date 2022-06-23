/*
 *
 *      Author: venture
 */
#pragma once

#include <assert.h>
#include <string>
#include <stdint.h>
#include <cstring>

namespace fix
{
    template <typename T, T MAX_N = INT32_MAX>
    struct fix_id_alloc
    {
    public:
        fix_id_alloc() : _pfreeids(nullptr), _nall(0), _nalloc(0), _pflags(nullptr)
        { }
        fix_id_alloc(T nCount, T beginid = 0) : _pfreeids(nullptr), _nall(0), _nalloc(0), _pflags(nullptr)
        {
            init(nCount, beginid);
        }

        ~fix_id_alloc()
        {
            release();
        }
        bool    init(T nCount, T beginid = 0)
        {
            size_t nNum = (size_t) nCount;
            if (nCount <= 0 || nCount >= MAX_N
                || beginid >= MAX_N || beginid < 0)
            {
                return false;
            }
            _beginid = beginid;
            _nall = nCount;
            _nalloc = 0;
            _pfreeids = new T[nNum];
            for (T i = 0; i < nCount; ++i)
            {
                _pfreeids[i] = i;
            }
            _pflags = new bool[nNum];
            memset(_pflags, 0, sizeof(_pflags[0]) * nNum);

            return true;
        }
        void    release()
        {
            if (_pfreeids != nullptr)
                delete[] _pfreeids;
            if (_pflags != nullptr)
                delete[] _pflags;

            _nall = 0;
            _nalloc = 0;
            _pfreeids = nullptr;
            _pflags = nullptr;
        }
        bool full()
        {
            return _nalloc == _nall;
        }

        T	alloc()
        {
            T id = MAX_N;
            if (_nalloc < _nall)
            {
                id = _pfreeids[_nalloc++];
                _pflags[id] = true;
                id += _beginid;
            }
            return id;
        }
        void    free(T id)
        {
            if (id < _beginid)
                return;
            id = id - _beginid;
            if (id >= _nall)
            {
                return;
            }
            if (_nalloc > 0)
            {
                if (_pflags[id])
                {
                    _pfreeids[--_nalloc] = id;
                    _pflags[id] = false;
                }
            }
        }

        bool    can_used(T id)
        {
            if (id < _beginid)
                return false;
            id = id - _beginid;
            if (id >= _nall)
            {
                return false;
            }

            if (_pflags[id])
            {
                return true;
            }
            else
            {
                if (_nalloc >= _nall)
                    return false;

                if (*(_pfreeids + _nalloc) != id)
                {
                    T* pRet = std::find(_pfreeids + _nalloc, _pfreeids + _nall - 1, id);
                    if (pRet == nullptr || *pRet != id) return false;
                    std::swap(*(_pfreeids + _nalloc), *pRet);
                }

                _nalloc++;
                _pflags[id] = true;
                return true;
            }
            return false;
        }

        T	size() { return _nalloc; }
        T	max_size() { return MAX_N; }

        bool is_valid(T id)
        {
            if (id == MAX_N || id < _beginid)
                return false;
            id = id - _beginid;
            if (id >= _nall)
            {
                return false;
            }
            return true;
        }

        void to_string(std::string& out)

        {
            out = out + "all:" + std::to_string(_nall) + ", "
                + "alloc:" + std::to_string(_nalloc) + ", "
                + "free:" + std::to_string(_nall - _nalloc) + "\n";


            out = out + "[alloc_list]: ";
            for (T i = 0; i < _nall; ++i)
            {
                if (_pflags[i])
                {
                    out = out + std::to_string(i + _beginid) + " ";
                }
            }
            out += "\n";


            out += "[free_list]: ";
            for (T i = _nalloc; i < _nall; ++i)
            {
                out = out + std::to_string(_pfreeids[i] + _beginid) + " ";
            }
            out += "\n";


            for (T i = _nalloc; i < _nall; ++i)
            {
                assert(!_pflags[_pfreeids[i]]);
            }
            T nCalcFreeCount = 0;
            for (T i = 0; i < _nall; ++i)
            {
                if (!_pflags[i])
                {
                    ++nCalcFreeCount;
                }
            }
            assert(nCalcFreeCount == (_nall - _nalloc));

        }
    private:
        T*      _pfreeids = nullptr;
        T       _nall = 0;
        T       _nalloc = 0;
        bool*   _pflags = nullptr;
        T		_beginid = 0;
    };
}