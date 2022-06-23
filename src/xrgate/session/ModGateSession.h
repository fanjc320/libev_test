#pragma once
#include "dtype.h"
#include "server/SvrModuleBase.h"
#include "util/SingleTon.h"

class ModGateSession : public SvrModuleBase, public SingleTon<ModGateSession>
{
    ModGateSession() = default;
    friend SingleTon;
public:

    virtual bool LogicInit();
    virtual void LogicFini();

    virtual void OnFrame(uint64_t qwTm) override;
    virtual void OnTick(uint64_t qwTm) override; //“ª∫¡√Î“ª¥Œ
};

