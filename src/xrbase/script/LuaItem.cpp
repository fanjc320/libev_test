/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "LuaItem.h"
#include "log/LogMgr.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

static int l_Info(lua_State* L)
{
    const char* szData = luaL_checkstring(L, 1);
    if (szData)
    {
        LOG_INFO(szData);
    }

    return 0;
}

static int l_Error(lua_State* L)
{
    const char* szData = luaL_checkstring(L, 1);
    if (szData)
    {
        LOG_ERROR(szData);
    }

    return 0;
}

static const luaL_Reg ExportLogMgr[] =
{
    { "LOG_INFO",		l_Info },
    { "LOG_ERROR",		l_Error },
    { nullptr, nullptr }
};

LUALIB_API int LuaExportLib(lua_State* L)
{
    luaL_newlib(L, ExportLogMgr);
    return 1;
}
