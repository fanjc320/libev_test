#include "pch.h"
#include "log/LogMgr.h"
#include "ModConfig.h"
#include "server/SvrAppBase.h"

ModConfig::ModConfig() : SvrModuleBase("ModConfig")
{

}

bool ModConfig::LogicInit()
{
    return SvrModuleBase::LogicInit();
}

void ModConfig::LogicFini()
{
    SvrModuleBase::LogicFini();
}

bool ModConfig::ResInit()
{
    return SvrModuleBase::ResInit();
}

bool ModConfig::ResAssembler()
{
    bool ret;
    ret = _const.Init();
    ERROR_LOG_EXIT0(ret);

    ret = _app.Init(GetMainApp()->GetConfigBase());
    ERROR_LOG_EXIT0(ret);
    return true;
Exit0:
    return false;
}

bool ModConfig::Reload()
{
    return SvrModuleBase::Reload();
}
