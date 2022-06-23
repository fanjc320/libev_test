/*
 *
 *      Author: venture
 */
#include "dtype.h"
#include "LoadConfigMgr.h"
#include "game_data.h"
#include "LoadConfigCsv.h"
#include "log/LogMgr.h"
#include "LoadConfigScript.h"

bool LoadConfigMgr::Init()
{
    bool ret = LoadConfigScript::Init();
    ERROR_LOG_EXIT0(ret);
    return true;
Exit0:
    LOG_ERROR("Config Load error!!!");
    return false;
}

bool LoadConfigMgr::InitAllFile()
{
    for (auto& itr : _mapConfig)
    {
        LOG_CUSTOM("Config", "Init class %s, file: %s", itr->GetObjName().c_str(), itr->GetPath().c_str());
    }
    return true;
Exit0:
    LOG_ERROR("InitAllFile error!!!");
    return false;
}

bool LoadConfigMgr::Load()
{
    bool ret = false;
    ret = _LoadAll();
    ERROR_LOG_EXIT0(ret);
    ret = _LoadAllEnd();
    ERROR_LOG_EXIT0(ret);
    return true;
Exit0:
    LOG_ERROR("Config Load error!!!");
    return false;
}

bool LoadConfigMgr::Reload()
{
    bool ret = false;
    ret = _LoadAll();
    ERROR_LOG_EXIT0(ret);
    ret = _LoadAllEnd();
    ERROR_LOG_EXIT0(ret);
    return true;
Exit0:
    LOG_ERROR("Config Reload error!!!");
    return false;
}

void LoadConfigMgr::AddLoadConfigBase(LoadConfigBase* pBase)
{
    _mapConfig.insert(pBase->GetObjName(), pBase);
}

LoadConfigBase* LoadConfigMgr::GetConfigBase(const std::string& strName)
{
    LoadConfigBase** p = _mapConfig.find(strName);
    if (p == nullptr)
        return nullptr;
    else
        return *p;
}

bool LoadConfigMgr::_LoadAll()
{
    bool ret = false;
    for (auto& itr : _mapConfig)
    {
        ret = itr->Load();
        LOG_CUSTOM("Config", "Load class %s, file: %s, ret: %d", itr->GetObjName().c_str(), itr->GetPath().c_str(), ret);
        if (!ret) return false;
    }
    return true;
}

bool LoadConfigMgr::_LoadAllEnd()
{
    bool ret = false;
    for (auto& itr : _mapConfig)
    {
        ret = itr->LoadEnd();
        LOG_CUSTOM("Config", "Load End class %s, file: %s, ret:%d", itr->GetObjName().c_str(), itr->GetPath().c_str(), ret);
        if (!ret) return false;
    }
    return true;
}
