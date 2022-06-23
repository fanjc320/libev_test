#pragma once
#include "server/SvrLogicHandle.h"
#include "util/SingleTon.h"

class GateHandle : public SvrLogicHandle, public SingleTon<GateHandle>
{ 
    GateHandle() = default;
    friend SingleTon;
public:
    bool LogicInit(LogicThread* pLogicThread);

    void OnFrame(uint64_t qwTick);
    void OnTick(uint64_t qwTick);
    bool OnReload();
    bool OnStop();
    void Fini();

    virtual bool OnHandleMsg(const MsgHead* pMsg, const HostHd& hd) override;
    bool OnModLogin(const MsgHead* pMsg, const HostHd& hd);
};

