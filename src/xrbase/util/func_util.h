/*
 *
 *      Author: venture
 */
#pragma once

#include <math.h>
#include <assert.h>
#include <string>
#include <ctime>

using namespace std;

// 生成UUID
extern int64_t gen_id();

///////////////////////////////////////////
///md5编码
////////////////////////////////////////////
//32位md5
extern std::string make_md5(const std::string &str);
extern std::string make_md5(const char* str, size_t len);
//16位md5
extern std::string make_md5_16(const std::string &str);
extern std::string make_md5_16(const char* str, size_t len);
