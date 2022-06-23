/*
 *
 *      Author: venture
 */
#pragma once
/************************************************************************/
/*          用于权重的数组随机                                          */
/************************************************************************/

/*  //整数
 *  rand_mgr<uint32> r;
 *  r.add(1, 10);
 *  r.add(2, 20);
 *  r.add(3, 30);
 *  uint32 n;
 *  std::vector<uint32> v;
 *  bool b = r.rand(n);
 *  b = r.rand_more(v, 2);
 *  b = r.rand_more(v, 3);
 *  b = r.rand_mode_unique(v, 2);
 *  b = r.rand_mode_unique(v, 3);
 *  //结构体
 *  rand_mgr<sItem> rx;
 *  rx.add(sItem(1, 10), 10);
 *  rx.add(sItem(2, 10), 20);
 *  rx.add(sItem(3, 10), 30);
 *  sItem nt;
 *  std::vector<sItem> vt;
 *  b = rx.rand(nt);
 *  b = rx.rand_more(vt, 2);
 *  b = rx.rand_more(vt, 3);
 *  b = rx.rand_mode_unique(vt, 2);
 *  b = rx.rand_mode_unique(vt, 3);
*/
#include <vector>
#include <algorithm>
#include "dtype.h"
#include "math/math_func.h"
template<typename T>

class rand_mgr
{
protected:
    struct rand_item
    {
        uint32_t weight = 0;
        uint32_t add_weight = 0;
        T      data;

        bool operator > (rand_item& val)
        {
            return weight > val.weight;
        }
        bool operator >= (rand_item& val)
        {
            return weight >= val.weight;
        }
    };

public:
    rand_mgr() {}
    ~rand_mgr() {}

    //添加数据和权重
    virtual bool add(const T& item, const uint32_t& weight)
    {
        rand_item a;
        a.data = item;
        a.weight = weight; 
        _all_weight += weight;  
        a.add_weight = _all_weight;
        _vec.emplace_back(a);
        return true;
    }

    //单一的随机
    virtual bool rand(T& item)
    {
        if (_all_weight <= 0) return false;

        uint32_t weight = rand_x(_all_weight);
        rand_item val;
        val.weight = weight;
        auto iter = std::upper_bound(_vec.begin(), _vec.end(), val);
        if (iter != _vec.end())
        {
            item = *iter.data;
            return true;
        }
        return false;
    }

    //返回的去重的随机
    virtual bool rand_mode_unique(std::vector<T>& ret, uint32_t num)
    {
        if (_all_weight <= 0) return false;

        ret.clear();
        if (num == 0 || _vec.size() == 0) return true;
        ret.reserve(num);

        if (num >= _vec.size())
        {
            for (const rand_item &data : _vec)
                ret.emplace_back(data.data);
            return true;
        }

        uint32_t weight = 0;
        std::vector<rand_item> vtemp;
        vtemp.assign(_vec.begin(), _vec.end());
        uint32_t allw = _all_weight;
		rand_item val;

        for (size_t i = 0; i < num; ++i)
        {
            if (allw == 0 || vtemp.size() == 0) return true;
			weight = rand_x(allw);
			val.weight = weight;
			auto iter = std::upper_bound(vtemp.begin(), vtemp.end(), val);
			if (iter != vtemp.end())
			{
                rand_item& data = *iter;
				ret.emplace_back(data.data);
                allw = (allw >= data.weight) ? (allw - data.weight) : 0;
                for (auto it = iter; it != vtemp.end(); ++it)
                {
                    *it->add_weight -= data.weight;
                }
				vtemp.erase(iter);
			}
        }
        return true;
    }

    //返回的不去重的随机
    virtual bool rand_mode(std::vector<T>& ret, uint32_t num)
    {
        if (_all_weight <= 0) return false;
        ret.clear();
        if (num == 0 || _vec.size() == 0) return true;
        ret.reserve(num);

        uint32_t weight = 0;	
        rand_item val;
        for (size_t i = 0; i < num; ++i)
        {
            weight = rand_x(_all_weight);
			val.weight = weight;
			auto iter = std::upper_bound(_vec.begin(), _vec.end(), val);
            if (iter != _vec.end())
            {
                ret.emplace_back(*iter.data);
                break;
            }
        }
        return true;
    }
private:
    std::vector<rand_item> _vec;
    uint32_t               _all_weight = 0;
};

