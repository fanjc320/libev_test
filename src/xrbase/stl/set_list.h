/*
 *      ≈≈–Úset, ºØ≥…list
 *      Author: venture
 */
#pragma once

#include <list>
#include <algorithm>
#include <iterator>
#include "dtype.h"

template<typename T>
class set_list
{
public:
    set_list() { }
    ~set_list(){}

    typedef typename std::list<T>::iterator ITERATOR;
    bool insert(const T& value)
    {
        if (_setData.find(value) != _setData.end())
        {
            throw("exsit!!");
            return true;
        }
        
        auto itrAdd = _lstID.insert(_lstID.end(), value);
        _setData[value] = itrAdd;

        return true;
    }

    bool exsit(T value)
    {
        if (_setData.find(value) != _setData.end())
            return true;
        return false;
    }

    void erase(T value)
    {
        auto itr = _setData.find(value);
        if (itr == _setData.end()) return;
        ITERATOR itrDel = itr->second;
        _setData.erase(value);
        _lstID.erase(itrDel);
    }

    ITERATOR erase(ITERATOR iter)
    {
        if (iter == _lstID.end()) return _lstID.end();
        T val = *iter;
        _setData.erase(val);
        return _lstID.erase(iter);
    }

    ITERATOR begin() { return _lstID.begin(); }
    ITERATOR end() { return _lstID.end(); }

    const std::list<T>& list() { return _lstID; }

    size_t size() { return _lstID.size(); }
    void clear()
    {
        _setData.clear();
        _lstID.clear();
    }

    T pop_front()
    {
        T p;
        auto itr = _lstID.begin();
        if (itr != _lstID.end())
        {
            p = *itr;
            _setData.erase(p);
            _lstID.erase(itr);
        }
        return p;
    }

private:
    umap<T, ITERATOR> _setData;
    std::list<T>      _lstID;
};

