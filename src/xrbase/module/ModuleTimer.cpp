#include "pch.h"
#include "ModuleTimer.h"
#include "log/LogMgr.h"
#include "time/timer_wheel.h"

ModuleTimer::ModuleTimer()
    : SvrModuleBase("ModuleTimer")
{

}

bool ModuleTimer::LogicInit()
{
    timer_wheel::init_type_name();
    bool ret = timer_wheel::init(IsResume());
    return true;
Exit0:
    return false;
}

void ModuleTimer::LogicFini()
{
    SvrModuleBase::LogicFini();
}

void ModuleTimer::OnFrame(uint64_t qwTm)
{
    SvrModuleBase::OnFrame(qwTm);
}

void ModuleTimer::OnTick(uint64_t qwTm)
{
    timer_wheel::main_loop();
}

bool ModuleTimer::Reload()
{
    //重新加载时间偏移量
    bool bRet = timer_wheel::reload();
    ERROR_LOG_EXIT0(bRet);
    return true;
Exit0:
    return false;
}
