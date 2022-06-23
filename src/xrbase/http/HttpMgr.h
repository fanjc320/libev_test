/*
 *
 *      Author: venture
 */

#pragma once
#include "http/HttpClient.h"
#include <list>
#include <stdint.h>
#include <functional>
#include <thread>
#include <mutex>
#include <vector>
#include "http/HttpParam.h"

using namespace std;

typedef std::function<void(HttpParam& req)> FUNC_HTTP_RES;
class HttpMgr
{
public:
    HttpMgr();
    ~HttpMgr();

    bool Init(uint16_t nMaxThread = 1);

    void RegFunc(FUNC_HTTP_RES func);
    void PushPost(HttpParam& data);
    void PushPost(HttpParam* pData);

    void Stop() { _bStop = true; }

    void MainLoop(time_t tmTick);

protected:
    void _AddRet(HttpParam* pData);

private:
    static void ThreadFunc(HttpMgr* pThis);
    bool			 _bStop = false;

    size_t			 _wThreadNum = 1;
    std::vector<std::thread> _vecThread;

    list<HttpParam*>  _list;
    list<HttpParam*>  _listRet;
    mutex             _lockRet;
    mutex   		  _lockPush;
    FUNC_HTTP_RES     _func;
};