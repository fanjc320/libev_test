/*
 *
 *      Author: venture
 */
#pragma once

#include "dtype.h"
#include <string>
#include <stdint.h>
#include <vector>
#include <algorithm>
#include "math/math_func.h"
#include "google/protobuf/message.h"

extern int pb2_string(const pb::Message& pb_msg, std::string& out_str, const std::string filter = "|");
extern int pb2_json(const pb::Message& pb_msg, std::string& out_str);

//实现 RepeatedPtrField<T> 转化输出 vector<T*>, 支持去重
template<typename T>
inline void pb_repeated_vec(const pb::RepeatedPtrField<T>& src, std::vector<const T*>& des, bool unique = false)
{
    des.reserve(src.size());

    for (size_t i = 0; i < src.size(); i++)
    {
        des.push_back(&(src.Get(i)));
    }

    if (unique)
        unique_vector(des);
}

//实现 RepeatedPtrField<T> 转化输出 vector<T>, 支持去重
template<typename T>
inline void pb_repeated_vec_t(const pb::RepeatedPtrField<T>& src, std::vector<T>& des)
{
    des.reserve(src.size());
    for (size_t i = 0; i < src.size(); i++)
    {
        des.push_back(src.Get(i));
    }
}

//实现 T* list 转化输出 vector<T*>, 支持去重
template<typename T>
inline void pb_repeated_vec(const T* src, size_t sz, std::vector<const T*>& des, bool unique = false)
{
    des.reserve(sz);

    for (size_t i = 0; i < sz; i++)
    {
        des.push_back(src + i);
    }
    if (unique)
        unique_vector(des);
}

//实现 RepeatedField<T> 复制到 T[] 数组
template<typename T>
inline void pb_repeated_copy(const pb::RepeatedField<T>& src, T* des)
{
    for (size_t i = 0; i < src.size(); ++i)
        *(des + i) = src.Get(i);
}