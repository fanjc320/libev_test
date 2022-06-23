#pragma once

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <string_view>

#define SAFE_DELETE(p) if((p != nullptr)){delete p; p = nullptr;};
#define SAFE_DELETE_ARRAY(p) if((p != nullptr)){delete[] p; p = nullptr;};
#define _S(T) #T

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#ifndef _MSC_VER
#ifndef BOOL
typedef int32_t BOOL;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#endif
#endif // _MSC_VER

#define uset std::unordered_set
#define umap std::unordered_map

#define MAP_FIND_NULL(map, key) \
        auto itrFind = (map).find(key); \
        if (itrFind != (map).end()) return itrFind->second; \
        return nullptr;

#define MAP_FIND_DEF(map, key, def) \
        auto itrFind = (map).find(key); \
        if (itrFind != (map).end()) return itrFind->second; \
        return (def); 

#define MAP_FIND_NUM(map, key) MAP_FIND_DEF(map, key, 0)
#define MAP_FIND_STR(map, key) MAP_FIND_DEF(map, key, "")

#ifndef ARR_LEN
#define ARR_LEN(a) (sizeof(a) / sizeof(*(a)))
#endif

namespace google
{
    namespace protobuf
    {
    }
}

#define CLASS_NODE_CAST(class_ptr__, __member__, __node_ptr__)  \
	        ((class_ptr__)((char*)(__node_ptr__) - (char*)&(((class_ptr__)0)->__member__)))

#define OFFSET2PTR(__node_ptr__, offset) ((int64_t)__node_ptr__ + (int64_t)(offset))

#define PTR2OFFSET(__node_ptr__, ptr)    ((int64_t)(ptr) - (int64_t)__node_ptr__)

#ifdef _MSC_VER
//#pragma warning (disable : 4244) // double->float float->double, INT64->INT32
//#pragma warning (disable : 4100)//未引用的形参
//#pragma warning (disable : 4800)//int->bool
//#pragma warning (disable : 4389)// == unsigned/signed
#pragma warning (disable : 4996)//strcpy declared
//#pragma warning (disable : 4018)//unsigned/signed
#pragma warning (disable : 4512)//assignment operator could not be generated
//#pragma warning (disable : 4627)//跳过预编译头的检测
#pragma warning (disable : 4099)//.pdb not found
//#pragma warning (disable : 4189)// local variable is init, but not referenced
#pragma warning (disable : 4091)//“typedef ” : 没有声明变量时忽略“”的左侧
#pragma warning (disable : 4102)//未引用的标签
#pragma warning (disable : 4200)//使用了非标准扩展: 结构/联合中的零大小数组
#pragma warning (disable : 4102)//未引用的标签
//#pragma warning (disable : 4099)//没有找调试信息
#pragma warning (disable: 26812)//警告 C26812：首选 "enum class" over "enum" (Enum)
#endif // _MSC_VER
// namespace google
namespace pb = ::google::protobuf;
