#pragma once
#include "server/SvrLogicHandle.h"
#include "util/SingleTon.h"

class ClientReconnHandle : public SvrLogicHandle, public SingleTon<ClientReconnHandle>
{ 
    ClientReconnHandle() = default;
    friend SingleTon;

public:
    bool Init(LogicThread* pLogicThread);
    void OnFrame(uint64_t qwTick);
    void OnTick(uint64_t qwTick);

    bool OnReload();
    bool OnStop();
    void Fini();
};

