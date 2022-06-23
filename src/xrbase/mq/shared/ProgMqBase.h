#pragma once
#include "dtype.h"
#include "shm/shm_x.h"
#include "text/ring_buf.h"

class ProgMqBase
{
public:
    virtual ~ProgMqBase();

    bool InitCreate(const std::string& szFrom, const std::string& szTo, int32_t max_buf, FUN_RING_BUF func_pop = nullptr);
    bool InitLoad(const std::string& szFrom, const std::string& szTo, FUN_RING_BUF func_pop = nullptr, int32_t max_buf = 0);
    bool InitLoad();

    void Fini();
    void SetPopFunc(FUN_RING_BUF func_pop);

    virtual bool Write(const unsigned char* buf, int32_t len);
    virtual void MainLoop(int32_t times = 100);

    const std::string& GetFromName() const { return _szFrom; }
    const std::string& GetToName()const { return _szTo; }
    const std::string& GetKey() const { return _szShmKey; }
    bool IsInit()  const { return _init; }
    const int64_t GetMaxSize() const { return _max_size; }
private:
    shm::HANDLE_T   _hdShm = nullptr;
    int32_t         _fdShm = -1;
    ring_buf*       _ring = nullptr;

    bool            _init = false;
    int32_t         _cur_pop_times = 0;

    std::string     _szFrom;
    std::string     _szTo;
    std::string     _szShmKey;
    int64_t         _max_size = 0;
    FUN_RING_BUF    _func_pop = nullptr;
};