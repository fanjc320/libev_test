/*
 *
 *      Author: venture
 */


#include "pch.h"
#include "time/time_func_x.h"
#include "time/FuncRunTime.h"
#include "HttpMgr.h"
#include "log/LogMgr.h"
#include "dtype.h"

HttpMgr::HttpMgr()
{
    // TODO Auto-generated constructor stub
}

HttpMgr::~HttpMgr()
{
    _bStop = true;
    for (auto itr = _vecThread.begin(); itr != _vecThread.end(); ++itr)
    {
        if (itr->joinable())
            itr->join();
    }
    _vecThread.clear();
    curl_global_cleanup();
}

bool HttpMgr::Init(uint16_t nMaxThread)
{
    if (CURLE_OK != curl_global_init(CURL_GLOBAL_ALL))
        return false;

    if (nMaxThread < 1) nMaxThread = 1;

    _wThreadNum = nMaxThread;
    _vecThread.resize(nMaxThread);

    for (uint16_t i = 0; i < _wThreadNum; ++i)
    {
        _vecThread[i] = std::thread(std::bind(HttpMgr::ThreadFunc, this));
    }
    return true;
}

void HttpMgr::RegFunc(FUNC_HTTP_RES func)
{
    _func = func;
}

void HttpMgr::PushPost(HttpParam& pData)
{
    HttpParam* pItem = new HttpParam(pData);
    _lockPush.lock();
    _list.push_back(pItem);
    _lockPush.unlock();
}

void HttpMgr::PushPost(HttpParam* pData)
{
    _lockPush.lock();
    _list.push_back(pData);
    _lockPush.unlock();
}

void HttpMgr::MainLoop(time_t tmTick)
{    
    list<HttpParam*> lstTemp;
    _lockRet.lock();
    lstTemp.swap(_listRet);
    _listRet.clear();
    _lockRet.unlock();

    for (auto itr = lstTemp.begin(); itr != lstTemp.end(); ++itr)
    {
        HttpParam* pItem = *itr;
        if (_func != nullptr)
        {
            _func(*pItem);
        }
        SAFE_DELETE(pItem);
    }
}

void HttpMgr::_AddRet(HttpParam* pData)
{
    _lockRet.lock();
    _listRet.push_back(pData);
    _lockRet.unlock();
}

void HttpMgr::ThreadFunc(HttpMgr* pThis)
{
    HttpClient htpClt;
    HttpParam* pItem = nullptr;
    string  strUrl, strFields;
    int32_t  nPreErr = 0;
    uint32_t nsleep = 0;
    TimeCount<120> tmrLog;
    size_t itemSize = 0;
    while (!pThis->_bStop)
    {
        pThis->_lockPush.lock();
        if (pThis->_list.size() == 0)
        {
            nsleep = (nsleep + 1) % 10 + 1;
            pThis->_lockPush.unlock();
            if (tmrLog.On(get_curr_time()))
            {
                LOG_CUSTOM("http", "size:0");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(nsleep));
            continue;
        }
        else
        {
            itemSize = pThis->_list.size();
            nsleep = 0;
            pItem = pThis->_list.front();
            pThis->_list.pop_front();
            pThis->_lockPush.unlock();

            if (tmrLog.On(get_curr_time()))
            {
                LOG_CUSTOM("http", "size:%d", itemSize);
            }

            strUrl = pItem->GetUrl();
            strFields = pItem->GetFields();
            if (!strUrl.empty())
            {
                pItem->GetStrRet().clear();
                int32_t nRc = 0;
                for (uint8_t i = 0; i < 60; ++i)
                {
                    try
                    {
                        if (pItem->IsPost())
                            nRc = htpClt.PostFile(strUrl, pItem->GetMapFields(), pItem->GetFile(), pItem->GetStrRet());
                        else
                            nRc = htpClt.Get(strUrl, pItem->GetStrRet());

                        if (nRc == CURLE_OK)
                        {
                            nPreErr = 0;
                            break;
                        }
                        else
                        {
                            LOG_CUSTOM("httpError", "Error: %u url:%s", nRc
                                , strUrl.c_str());
                            if (nPreErr >= 5)
                                break;
                        }
                    }
                    catch (std::exception& exp)
                    {
                        LOG_CUSTOM("httpError", "Error: %u url:%s err:%s", nRc
                            , strUrl.c_str(), exp.what());
                    }
                    catch (...)
                    {
                        LOG_CUSTOM("httpError", "Error: %u url:%s", nRc
                            , strUrl.c_str());
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

                pItem->SetRetCode(nRc);
                if (nRc != CURLE_OK)
                {
                    pItem->AddErrTimes();
                    ++nPreErr;
                    if (pItem->GetErrTimers() > 5)
                    {
                        pThis->_AddRet(pItem);
                    }
                    else
                        pThis->PushPost(pItem);
                }
                else
                {
                    if (!pItem->IsRet())
                    {
                        delete pItem;
                        pItem = nullptr;
                    }
                    else
                        pThis->_AddRet(pItem);
                }
            }
        }
    }
}
