/*
 *
 *      Author: venture
 */
#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include "platform/spthread.h"
#include <list>
#include "time/TimeCount.h"
#include "util/SingleTon.h"
#include "db/base/DbDef.h"

class DbThread
{
public:
    DbThread(void);
    virtual ~DbThread(void);

public:
    virtual bool Init();
    virtual void UnInit();
    static unsigned SP_THREAD_CALL update(void*);
    virtual void PushData(DbRequest& dbOperate);
    virtual void PushData(DbRequest* pItem);

    virtual bool PopData();
    virtual void MainLoop(time_t tmTick) = 0;
    virtual int64_t Exec(DbRequest& oRequest) = 0;

protected:
    std::list<DbRequest*>   _dbOpList;
    std::mutex			    _lock;
    std::thread			    _thread;
    TimeCount<120>          _tmrLog;
    bool				    _bExit = false;
};
