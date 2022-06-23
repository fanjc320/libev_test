#pragma once
#include "dtype.h"
#include "server/SvrModuleBase.h"
#include "util/SingleTon.h"
#include "config/ConfigStruct.h"
#include "config/AppConfig.h"
#include "config/ConstConfig.h"

class ModConfig : public SvrModuleBase, public SingleTon<ModConfig>
{
    ModConfig();
    friend SingleTon;
public:
    virtual bool LogicInit() override;
    virtual void LogicFini() override;

    virtual bool ResInit()override;
    virtual bool ResAssembler() override;

    virtual bool Reload() override;

    ConstConfig& GetConst() { return _const; }
    AppConfig& GetApp() { return _app; }

private:
    ConstConfig _const;
    AppConfig   _app;
};