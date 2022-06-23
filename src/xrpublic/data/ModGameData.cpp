#include "pch.h"
#include "tolua++.h"
#include "data/game/item_base.h"
#include "data/game/item_skill.h"
#include "ModGameData.h"
#include "data/LoadConfigMgr.h"
#include "log/LogMgr.h"
#include "data/game_data_lua.h"
#include "data/LoadConfigScript.h"
#include "data/LoadConfigCsv.h"
#include "data/game_data_lua.h"

ModGameData::ModGameData() : SvrModuleBase("ModGameData")
{

}

bool ModGameData::LogicInit()
{
    return SvrModuleBase::LogicInit();
}

void ModGameData::LogicFini()
{
    SvrModuleBase::LogicFini();
}

bool ModGameData::ResInit()
{    
    int ret = 0;
    bool bRet = LoadConfigScript::RunToLuaFunc(tolua_game_data_lua_open);
    ERROR_LOG_EXIT0(bRet);

#include "data/game/game_load_def.h"
    return true;
Exit0:
    return false;
}

bool ModGameData::ResAssembler()
{
    auto mgr = LoadConfigMgr::GetInstance()->GetConfigBase(_S(item_base));
    const item_base* p = mgr->GetBaseData<item_base>(1);

    return SvrModuleBase::ResAssembler();
}

bool ModGameData::Reload()
{
    return SvrModuleBase::Reload();
}
