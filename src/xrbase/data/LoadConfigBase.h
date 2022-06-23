/*
 *
 *      Author: venture
 */
#pragma once
#include "dtype.h"
#include "game_data.h"

class LuaScript;
class LoadConfigBase
{ 
public:
    virtual bool Init(const std::string& szPath, const std::string& objName);
    virtual bool Unit();
    virtual bool Load();
    virtual bool LoadEnd();

    virtual const std::string& GetObjName() { return _objName; }// class name
    virtual const std::string& GetPath() { return _szPath; }

    template<typename T>
    const T* GetBaseData(int64_t key);

    const umap<int64_t, game_data*>& GetMap() { return _mapVal;}

protected:
    std::string              _objName; //class name
    std::string              _szPath;
    umap<int64_t, game_data*> _mapVal;
};

template<typename T>
const T* LoadConfigBase::GetBaseData(int64_t key)
{
    auto itr = _mapVal.find(key);
    if (itr != _mapVal.end())
        return dynamic_cast<T*>(itr->second);
    return nullptr;
}

