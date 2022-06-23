/*
 *
 *      Author: venture
 */
#pragma once
#include "dtype.h"
#include "tolua++.h"

struct game_data;
class LuaScript;
class LoadConfigBase;
struct lua_state;

typedef int (*TO_LUA_FUNC)(lua_State*);

class LoadConfigScript
{ 
public:
    static bool Init();

    static bool LoadFromFile(game_data* pItem, const char* objName,
        const char* type, const char* name, const char* value);
    static LuaScript* GetScript();
    static bool RunToLuaFunc(TO_LUA_FUNC func);

private:
    static bool _load_from_file(game_data* pItem, const char* objName,
        const char* type, const char* name, const char* value);
    static bool _load_from_file(game_data* pItem, const char* objName,
        const char* type, const char* name, int32_t index, const char* value);
};

