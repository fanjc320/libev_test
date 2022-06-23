#include "pch.h"
#include "ModuleData.h"
#include "data/LoadConfigMgr.h"
#include "log/LogMgr.h"

ModuleData::ModuleData() : SvrModuleBase("ModuleData")
{

}

bool ModuleData::LogicInit()
{
    return SvrModuleBase::LogicInit();
}

void ModuleData::LogicFini()
{
    SvrModuleBase::LogicFini();
}

bool ModuleData::ResInit()
{
    bool bRet = false;
    bRet = LoadConfigMgr::GetInstance()->Init();
    ERROR_LOG_EXIT0(bRet);
    return true;
Exit0:
    return false;
}

bool ModuleData::ResAssembler()
{
    bool bRet = LoadConfigMgr::GetInstance()->InitAllFile();
    ERROR_LOG_EXIT0(bRet);
    bRet = LoadConfigMgr::GetInstance()->Load();
    ERROR_LOG_EXIT0(bRet);
    return true;
Exit0:
    return false;
}

bool ModuleData::Reload()
{
    bool bRet = LoadConfigMgr::GetInstance()->Reload();
    if (!bRet)
    {
        LOG_ERROR("Reload Error!!!");
        return false;
    }

    return true;
}
