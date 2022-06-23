/*
 *
 *      Author: venture
 */
#include "LoadConfigBase.h"

bool LoadConfigBase::Init(const std::string& szPath, const std::string& objName)
{
    return true;
}

bool LoadConfigBase::Unit()
{
    for (auto& itr : _mapVal)
    {
        delete itr.second;
    }
    _mapVal.clear();
    return true;
}

bool LoadConfigBase::Load()
{
    return true;
}

bool LoadConfigBase::LoadEnd()
{
    bool ret = true;
    for (auto& itr : _mapVal)
    {
        ret = itr.second->read_end(itr.second->id);
        if (!ret) return false;
    }
    return true;
}