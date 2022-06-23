#pragma once
#include "dtype.h"
#include "server/SvrModuleBase.h"

class ModuleData : public SvrModuleBase
{
public:
    ModuleData();
    virtual bool LogicInit() override;
    virtual void LogicFini() override;

    virtual bool ResInit()override;
    virtual bool ResAssembler() override;

    virtual bool Reload() override;
};