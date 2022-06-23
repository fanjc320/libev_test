#pragma once
#include "dtype.h"
#include "server/SvrModuleBase.h"
#include "util/SingleTon.h"

class ModulePb : public SvrModuleBase, public SingleTon<ModulePb>
{
    ModulePb();
    friend SingleTon;
public:
    virtual bool LogicInit() override;
    virtual void LogicFini() override;
};