/*
 *
 *      Author: venture
 */

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <chrono>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include <vector>
#include <algorithm>
#include <string>

#ifndef _MSC_VER
#include <unistd.h>
#include <sys/time.h>
#include <netinet/in.h>
#define memcpy_s memcpy_safe
#define sprintf_s sprintf_safe
#define fprintf_s fprintf
#define printf_s printf
#define sscanf_s sscanf
#define strtok_s strtok_r
#define _strlwr_s strlwr_safe 
#define _strupr_s strupr_safe 
#define strcat_s strcat_safe
#define fopen_s fopen
#define localtime_s(Res, TIME_T) localtime_r(TIME_T, Res)
#define gmtime_s(Res, TIME_T) gmtime_r(TIME_T, Res)
#else
#define snprintf _snprintf
#endif // _MSC_VER

 //////////////////////////////////////////////////////////////////////////
 // Function name   : strncpy_safe
 // Description     : ��ȫ�ַ�����������
extern int strncpy_safe(char* szDst, size_t countofDst, const char* szSrc, size_t countofCopy);

// Function name   : strncpy_safe
// Description     : ��ȫ�ַ�����������������ʡ��Ŀ�������С
template<size_t S>
inline int strncpy_safe(char(&szDst)[S], const char* szSrc, size_t countofCopy)
{
    return strncpy_safe(szDst, S, szSrc, countofCopy);
}

// Function name   : strncpy_safe
// Description     : ��ȫ�ַ�����������������ʡ��Ŀ���Դ�����С
template<size_t S1, size_t S2>
inline int strncpy_safe(char(&szDst)[S1], const char(&szSrc)[S2])
{
    return strncpy_safe(szDst, S1, szSrc, S2);
}

// Function name   : strncpy_safe
// Description     : ��ȫ�ַ�����������������ʡ��Ŀ���Դ�����С
template<size_t S1>
inline int strncpy_safe(char(&szDst)[S1], const std::string& src)
{
    return strncpy_safe(szDst, S1, src.c_str(), src.size());
}

//////////////////////////////////////////////////////////////////////////
// Function name   : memcpy_safe
// Description     : ��ȫ�ڴ濽������
extern int memcpy_safe(void* pDst, size_t countofDst, const void* pSrc, size_t countofCopy);

// Function name   : sprintf_safe
// Description     : ��ȫ��ʽ���ַ�������
template<typename... Args>
inline int sprintf_safe(char* szDst, size_t countofDst, const char* szFormat, Args&&... args)
{
    int nLen = snprintf(szDst, countofDst, szFormat, std::forward<Args>(args)...);
    szDst[countofDst - 1] = 0;
    return nLen > (int) countofDst ? (int) countofDst : nLen;
}

// Function name   : sprintf_safe
// Description     : ��ȫ��ʽ���ַ�������������ʡ��Ŀ�������С
template<size_t S, typename... Args>
inline int sprintf_safe(char(&szDst)[S], const char* szFormat, Args&&... args)
{
    int nLen = snprintf(szDst, S, szFormat, std::forward<Args>(args)...);
    szDst[S - 1] = 0;
    return nLen > (int) S ? (int) S : nLen;
}

//////////////////////////////////////////////////////////////////////////
// Function name   : strlwr_safe
// Description     : ��ȫת����Сд����
extern int strlwr_safe(char* str, size_t numberOfElements);

// Function name   : strlwr_safe
// Description     : ��ȫת����Сд����
template<size_t S>
inline int strlwr_safe(char(&str)[S])
{
    return strlwr_safe(str, S);
};

// Function name   : strupr_safe
// Description     : ��ȫת���ɴ�д����
extern int strupr_safe(char* str, size_t numberOfElements);

// Function name   : strupr_safe
// Description     : ��ȫת���ɴ�д����
template<size_t S>
inline int strupr_safe(char(&str)[S])
{
    return strupr_safe(str, S);
};

// Function name   : strcat_safe
// Description     : ��ȫ����
extern int strcat_safe(char* strDestination, size_t numberOfElements, const char* strSource);

//ȫ�ַ����滻
extern std::string str_replace(const std::string& strReplaced, const std::string& origStr, const std::string& newStr);

//ȫ��ת��Сд
extern std::string str_to_lower(const std::string& str);

//���ִ�Сд�Ƚ�
extern int  strcmp_ncase(const std::string& strA, const std::string& strB);

extern std::string trim_all(const std::string& strReplaced);
extern std::string trim_s(const std::string& text);
extern std::string trim_left(const std::string& text);
extern std::string trim_right(const std::string& text);

extern std::string check_name(const std::string& text, size_t size = 32);
extern bool check_name_valid(const std::string& text);

//�����ַ����ж�
extern bool is_digit(std::string& text);
