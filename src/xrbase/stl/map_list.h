/*
 *      ≈≈–Úmap, ºØ≥…list
 *      Author: venture
 */
#pragma once

#include <list>
#include "dtype.h"
#include "log/LogMgr.h"

template <typename F, typename T>
class map_list
{
public:
    map_list() { }
    ~map_list() { }

    typedef typename std::list<T>::iterator ITERATOR;

    bool insert(F nIndex, const T& value)
    {
        T* p = find(nIndex);
        if (p != nullptr)
        {
            LOG_ERROR("exsit!!");
            return true;
        }
        auto itrAdd = _lstID.insert(_lstID.end(), value);
        _mapData[nIndex] = itrAdd;
        return true;
    }

    void erase(F nIndex)
    {
        auto itr = _mapData.find(nIndex);
        if (itr == _mapData.end()) return;
        ITERATOR itrDel = itr->second;
        _mapData.erase(itr);
        _lstID.erase(itrDel);
    }

    ITERATOR erase(F nIndex, ITERATOR iter)
    {
        if (iter == _lstID.end()) return _lstID.end();
        _mapData.erase(nIndex);
        return _lstID.erase(iter);
    }

    bool exsit(F nIndex)
    {
        return _mapData.find(nIndex) != _mapData.end();
    }

    T* find(F nIndex)
    {
        auto itr = _mapData.find(nIndex);
        if (itr == _mapData.end()) return nullptr;
        
        return &(*(itr->second));
    }

    const std::list<T>& list() { return _lstID; }
    const umap < F, ITERATOR>& map() { return _mapData; }

    void clear()
    {
        _mapData.clear();
        _lstID.clear();
    }

    size_t size()
    {
        return _lstID.size();
    }

    ITERATOR begin()
    {
        return _lstID.begin();
    }

    ITERATOR end()
    {
        return _lstID.end();
    }

private:
    std::list<T>        _lstID;
    umap<F, ITERATOR>   _mapData;
};
