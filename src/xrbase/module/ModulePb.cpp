#include "pch.h"
#include "ModulePb.h"
#include "log/LogMgr.h"
#include "server/SvrAppBase.h"
#include "pb/pb_parse.h"

ModulePb::ModulePb() : SvrModuleBase("ModuleLog")
{

}

bool ModulePb::LogicInit()
{
    set_pb_log_handle();
    return true;
}

void ModulePb::LogicFini()
{
    
}