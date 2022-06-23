/*
 *
 *      Author: venture
 */

#pragma once
#include "dtype.h"
#include "util/SingleTon.h"
#include "stl/map_list.h"

class LuaScript;
struct game_data;

class LoadConfigBase;
class LoadConfigMgr : public SingleTon<LoadConfigMgr>
{
    LoadConfigMgr() = default;
    friend SingleTon;
public:
    bool Init();
    bool InitAllFile();

    bool Load();
    bool Reload();

    void AddLoadConfigBase(LoadConfigBase* pBase);
    LoadConfigBase* GetConfigBase(const std::string& strName);

private:
    bool _LoadAll();
    bool _LoadAllEnd();

    map_list<std::string, LoadConfigBase*> _mapConfig;
};