/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <typeinfo>

#include "DbThread.h"
#include "time/time_func_x.h"
#include "log/LogMgr.h"
#include "dtype.h"

DbThread::DbThread(void)
{

}

DbThread::~DbThread(void)
{
    _bExit = true;
    if (_thread.joinable())
        _thread.join();

    if (_dbOpList.size() > 0)
        PopData();

    _dbOpList.clear();
}

bool DbThread::Init()
{
    _thread = std::thread(DbThread::update, this);
    return true;
}

void DbThread::UnInit()
{
    _bExit = true;
    PopData();
}

unsigned SP_THREAD_CALL DbThread::update(void* pdate)
{
    int i = 0;
    DbThread* pThis = (DbThread*) (pdate);
    TimeCount<60> tmPing;
    uint32_t nsleep = 0;
    while (!pThis->_bExit)
    {
        if (tmPing.On(get_curr_time()))
        {
            pThis->MainLoop(get_tick_count());
        }

        if (!pThis->PopData())
        {
            nsleep = (nsleep + 1) % 10 + 1;
            std::this_thread::sleep_for(std::chrono::milliseconds(nsleep));
        }
        else
        {
            if (pThis->_tmrLog.On(get_curr_time()))
            {
                LOG_CUSTOM("dblist", "%s, Size:0", typeid(*pThis).name());
            }
            nsleep = 0;
        }
    }

    return 0;
}

void DbThread::PushData(DbRequest& dbOperate)
{
    if (dbOperate.type() == eDBOptType::NONE)
    {
        LOG_ERROR("type is error, %s-%s!", dbOperate.tbl().c_str()
            , dbOperate.sectbl().name().c_str());
        return;
    }
    DbRequest* pItem = new DbRequest(dbOperate);
    //Log_Info("Push query sql table:%s type:%d", dbOperate.sec_tbl().c_str(), dbOperate.type());
    _lock.lock();
    _dbOpList.emplace_back(pItem);
    _lock.unlock();
    //Log_Info("Push query sql table:%s type:%d success", dbOperate.sec_tbl().c_str(), dbOperate.type());
}

void DbThread::PushData(DbRequest* pItem)
{
    _lock.lock();
    _dbOpList.emplace_back(pItem);
    _lock.unlock();
}

bool DbThread::PopData()
{
    std::list<DbRequest*> dbRunList;
    _lock.lock();
    if (_dbOpList.empty())
    {
        _lock.unlock();
        return false;
    }
    dbRunList.swap(_dbOpList);
    _lock.unlock();

    if (dbRunList.size() > 2000)
    {
        LOG_CUSTOM("dblist", "%s, Size:%u too big!", typeid(*this).name(), dbRunList.size());
    }

    if (_tmrLog.On(get_curr_time()))
    {
        LOG_CUSTOM("dblist", "%s, Size:%u", typeid(*this).name(), dbRunList.size());
    }
    size_t i = 0;

    for (auto itr = dbRunList.begin(); itr != dbRunList.end(); ++itr)
    {
        DbRequest* pItem = *itr;
        Exec(*pItem);
        ++i;
        if (i % 10000 == 0)
        {
            LOG_CUSTOM("dblist", "%s, Size:%u/%u", typeid(*this).name(), i, (dbRunList.size() + i));
        }
        SAFE_DELETE(pItem);
    }
    dbRunList.clear();

    return true;
}
