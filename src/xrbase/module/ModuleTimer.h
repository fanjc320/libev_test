#pragma once

#include "dtype.h"
#include "server/SvrModuleBase.h"
#include "util/SingleTon.h"

class ModuleTimer : public SvrModuleBase, public SingleTon<ModuleTimer>
{
    ModuleTimer();
    friend SingleTon;
public:
    virtual bool LogicInit() override;
    virtual void LogicFini() override;

    virtual void OnFrame(uint64_t qwTm) override;
    virtual void OnTick(uint64_t qwTm) override; //“ª∫¡√Î“ª¥Œ

    virtual bool Reload() override;
};