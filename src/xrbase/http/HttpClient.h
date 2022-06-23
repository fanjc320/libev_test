/*
 *
 *      Author: venture
 */

#pragma once

#include <curl/curl.h>
#include <string>
#include <stdint.h>
#include "dtype.h"

using namespace std;

class HttpClient
{
public:
    HttpClient(void);
    ~HttpClient(void);

public:

    int32_t Post(const string& strUrl, const string& strPost, std::string& strRes);
    int32_t PostFile(const string& strUrl, const umap<string, string>& mapFields
        , const string& strFile, std::string& strRes);

    int32_t PostSSL(const string& strUrl, const string& strPost
        , std::string& strRes, const string& strCaPath);

    int32_t Get(const string& strUrl, std::string& strRes);
    int32_t GetSSL(const string& strUrl, std::string& strRes
        , const string& strCaPath);

public:
    void SetDebug(bool bDebug) { _bDebug = bDebug; }
    static size_t OnWriteData(void* buffer, size_t size
        , size_t nmemb, void* lpVoid);
    static int32_t OnDebug(CURL* pURL, curl_infotype itype
        , char* pData, size_t size, void* lpVoid);

private:
    bool _bDebug = false;
};