#pragma once
#include "dtype.h"
#include "server/SvrModuleBase.h"
#include "util/SingleTon.h"

class ModGameData : public SvrModuleBase, public SingleTon<ModGameData>
{
    ModGameData();
    friend SingleTon;
public:
    virtual bool LogicInit() override;
    virtual void LogicFini() override;

    virtual bool ResInit()override;
    virtual bool ResAssembler() override;

    virtual bool Reload() override;
};