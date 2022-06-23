#include "pch.h"
#include <functional>
#include "ProgMqBase.h"
#include "platform/file_func.h"
#include "string/str_format.h"
#include "memory/malloc_x.h"
#include "log/LogMgr.h"
#include "server/SvrAppBase.h"
#include "math/math_basic.h"

ProgMqBase::~ProgMqBase()
{
    Fini();
}

bool ProgMqBase::InitCreate(const std::string& szFrom, const std::string& szTo, int32_t max_buf, FUN_RING_BUF func_pop)
{
    _func_pop = func_pop;
    _szFrom = szFrom;
    _szTo = szTo;

    int64_t max_create = (int64_t) max_buf + RING_HEAD_SIZE;
    max_create = ALIGN8(max_create);
    ERROR_LOG_EXIT0(max_create < INT32_MAX);
    _max_size = max_create;

    if (_szFrom.empty())
    {
        LOG_ERROR("_szFrom is empty !!!", szTo.c_str());
        return false;
    }

    _szShmKey = shm::shm_mk_key(_szFrom, szTo);

    if (_hdShm == nullptr)
    {
        _hdShm = shm::shm_create_x(_szShmKey, max_create, (void**) &_ring, _fdShm);
        ERROR_LOG_EXIT0(_hdShm != nullptr && _ring != nullptr);
    }

    _ring->init((int32_t) max_create, FALSE);
    _init = true;
    return true;

Exit0:
    Fini();
    return false;
}

bool ProgMqBase::InitLoad(const std::string& szFrom, const std::string& szTo, FUN_RING_BUF func_pop, int32_t max_buf)
{
    _func_pop = func_pop;
    _szFrom = szFrom;
    _szTo = szTo;
    if (_szFrom.empty() || _szTo.empty()) return false;

    _szShmKey = shm::shm_mk_key(_szFrom, szTo);
    int64_t qwSize = 0;
    if (_hdShm == nullptr)
    {
        _hdShm = shm::shm_open_x(_szShmKey, (void**) &_ring, qwSize, _fdShm);
        ERROR_LOG_EXIT0(_hdShm != nullptr && _ring != nullptr);
        ERROR_LOG_EXIT0(qwSize > 0 && qwSize < INT32_MAX);
        if (max_buf > 0)
        {
            int64_t max_create = (int64_t) max_buf + RING_HEAD_SIZE;
            max_create = ALIGN8(max_create);
            ERROR_LOG_EXIT0(max_create < INT32_MAX);
            _max_size = max_create;

            ERROR_LOG_EXIT0(_max_size == qwSize);
        }
        else
        {
            _max_size = qwSize;
        }
    }

    _init = true;
    return true;

Exit0:
    Fini();
    LOG_ERROR("InitLoad Error from %s to %s!", _szFrom.c_str(), _szTo.c_str());
    return false;
}

bool ProgMqBase::InitLoad()
{
    return InitLoad(_szFrom, _szTo, _func_pop);
}

void ProgMqBase::Fini()
{
    if (_hdShm != nullptr && _fdShm != -1)
        shm::shm_close_handle(_hdShm, _fdShm);

    _init = false;
    _hdShm = nullptr;
    _fdShm = -1;
    _ring = nullptr;
    _max_size = 0;
}

void ProgMqBase::SetPopFunc(FUN_RING_BUF func_pop)
{
    _func_pop = func_pop;
}

bool ProgMqBase::Write(const unsigned char* buf, int32_t len)
{
    if (_ring == nullptr) return false;
    return _ring->write(buf, len);
}

void ProgMqBase::MainLoop(int32_t times)
{
    if (_ring == nullptr) return;
    _ring->for_each(_func_pop, _cur_pop_times, 100);
}