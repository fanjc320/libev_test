/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "LuaScript.h"
extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include "tolua++.h"
#include "LuaItem.h"
#include <string>
#include "log/LogMgr.h"
#include <assert.h>
#include "platform/file_func.h"

extern int LuaExportLib(lua_State*);

LuaScript::LuaScript()
{

}

LuaScript::~LuaScript()
{
    _pLuaState = nullptr;
}

bool LuaScript::Init(const std::string& szPath)
{
    bool bResult = false;
    int nTopIndex = 0;

    _pLuaState = luaL_newstate();
    ERROR_LOG_EXIT0(_pLuaState);

    nTopIndex = lua_gettop(_pLuaState);

    luaL_openlibs(_pLuaState);
    _AddLuaSearchPath(szPath);

    luaL_requiref(_pLuaState, "export_interface", LuaExportLib, 0);

    ERROR_LOG_EXIT0(_CompileScripts(szPath));

    _szPath = szPath;
    bResult = true;
Exit0:
    if (_pLuaState)
    {
        lua_settop(_pLuaState, nTopIndex);
    }
    return bResult;
}

void LuaScript::UnInit()
{
    if (_pLuaState)
    {
        lua_close(_pLuaState);
        _pLuaState = nullptr;
    }
}

void LuaScript::_AddLuaSearchPath(const std::string& szPath)
{
    int		nTopIndex = 0;
    const char* szOldPath = nullptr;

    nTopIndex = lua_gettop(_pLuaState);

    lua_getglobal(_pLuaState, "package");
    lua_getfield(_pLuaState, -1, "path");
    szOldPath = lua_tostring(_pLuaState, -1);
    lua_pop(_pLuaState, 1);

    lua_pushfstring(_pLuaState, "%s;%s/?.lua", szOldPath, szPath.c_str());
    lua_setfield(_pLuaState, -2, "path");
    lua_pop(_pLuaState, 1);

    lua_settop(_pLuaState, nTopIndex);
}

bool LuaScript::_CompileScripts(const std::string& szPath)
{
    int		nTopIndex = 0;
    bool	bResult = false;

    nTopIndex = lua_gettop(_pLuaState);

    std::vector<std::string> vecFile;
    file::get_folder_file(szPath.c_str(), vecFile, true, "lua");
    size_t nDirLen = szPath.length();

    for (auto itr : vecFile)
    {
        std::string strName = itr.substr(0, itr.size() - 4);
        std::string subName = strName.substr(nDirLen + 1, strName.size() - nDirLen - 1);
        subName = str_replace(subName, "/", ".");
        ERROR_LOG_EXIT0(_Compile(subName));
    }

    bResult = true;

Exit0:
    lua_settop(_pLuaState, nTopIndex);
    return bResult;
}

bool LuaScript::_Compile(const std::string& szPath)
{
    char	szFile[512];

    sprintf_safe(szFile, sizeof(szFile), "require \'%s\'", szPath.c_str());
    if (luaL_dostring(_pLuaState, szFile))
    {
        LOG_ERROR(lua_tostring(_pLuaState, -1));
        return false;
    }
    return true;
}

int LuaScript::CallFunc(const char* szGlobalName, const char* szFuncname, const char* szArgFormat, ...)
{
    int nResult = 0;
    int nTopIndex = lua_gettop(_pLuaState);

    int nArgNum = 0;
    int nResNum = 0;

    ERROR_LOG_EXIT0(_pLuaState != nullptr);

    assert(szArgFormat);
    va_list valist;
    va_start(valist, szArgFormat);

    lua_getglobal(_pLuaState, szGlobalName);
    ERROR_LOG_EXIT0(lua_istable(_pLuaState, -1));

    lua_getfield(_pLuaState, -1, szFuncname);
    ERROR_LOG_EXIT0(lua_isfunction(_pLuaState, -1));

    for (nArgNum = 0; *szArgFormat; nArgNum++)
    {
        switch (*szArgFormat++)
        {
            case 'f':
                {
                    lua_pushnumber(_pLuaState, va_arg(valist, double));
                }
                break;

            case 'i':
                {
                    lua_pushinteger(_pLuaState, va_arg(valist, int32_t));
                }
                break;

            case 'l':
                {
                    lua_pushinteger(_pLuaState, va_arg(valist, int64_t));
                }
                break;

            case 's':
                {
                    lua_pushstring(_pLuaState, va_arg(valist, const char*));
                }
                break;

            case 'S':
                {
                    const char* szBuff = va_arg(valist, const char*);
                    lua_pushlstring(_pLuaState, szBuff, va_arg(valist, int32_t));
                }
                break;

            case 'E':
                {
                    LuaItem* pObj = va_arg(valist, LuaItem*);
                    if (nullptr == pObj)
                    {
                        lua_pushnil(_pLuaState);
                    }
                    else
                    {
                        lua_rawgeti(_pLuaState, LUA_REGISTRYINDEX, pObj->GetKey());
                    }
                }
                break;

            case 'v':
                {
                    vector<int>& vecData = *va_arg(valist, vector<int>*);
                    lua_createtable(_pLuaState, (int) vecData.size(), 0);
                    for (size_t i = 0; i < vecData.size(); i++)
                    {
                        lua_pushnumber(_pLuaState, vecData[i]);
                        lua_rawseti(_pLuaState, -2, i + 1);
                    }
                }
                break;
            case 'o':
                {
                    void* pObj = va_arg(valist, void*);
                    char* szClassName = va_arg(valist, char*);
                    tolua_pushusertype(_pLuaState, pObj, szClassName);
                }
                break;
            case '>':
                goto args;
        }
    }

args:

    nResNum = (int) strlen(szArgFormat);
    if (lua_pcall(_pLuaState, nArgNum, nResNum, 0))
    {
        LOG_ERROR(lua_tostring(_pLuaState, -1));
        goto Exit0;
    }

    nResNum = -nResNum;
    while (*szArgFormat)
    {
        switch (*szArgFormat++)
        {
            case 'f':
                {
                    double* val = va_arg(valist, double*);
                    if (lua_isnumber(_pLuaState, nResNum))
                    {
                        *val = (double) lua_tonumber(_pLuaState, nResNum);
                    }
                    else if (lua_isinteger(_pLuaState, nResNum))
                    {
                        *val = (double) lua_tointeger(_pLuaState, nResNum);
                    }
                    else
                        *val = 0.0;
                }
                break;

            case 'i':
                {
                    int32_t* val = va_arg(valist, int32_t*);
                    if (lua_isnumber(_pLuaState, nResNum))
                    {
                        LUA_NUMBER nRet = (LUA_NUMBER) (lua_tonumber(_pLuaState, nResNum));
                        *val = (int32_t) nRet;
                    }
                    else if (lua_isinteger(_pLuaState, nResNum))
                    {
                        *val = (int32_t) lua_tointeger(_pLuaState, nResNum);
                    }
                    else
                        *val = 0;
                }

                break;

            case 's':
                if (lua_isstring(_pLuaState, nResNum))
                {
                    *va_arg(valist, string*) = lua_tostring(_pLuaState, nResNum);
                }
                break;

            case 'l':
                {
                    int64_t* val = va_arg(valist, int64_t*);
                    if (lua_isnumber(_pLuaState, nResNum))
                    {
                        LUA_NUMBER nRet = lua_tonumber(_pLuaState, nResNum);
                        *val = (int64_t) nRet;
                    }
                    else if (lua_isinteger(_pLuaState, nResNum))
                    {
                        *val = (int64_t) lua_tointeger(_pLuaState, nResNum);
                    }
                    else
                        *val = 0;
                }
                break;

            default:
                {
                    goto Exit0;
                }
        }

        nResNum++;
    }

    nResult = 1;
Exit0:
    va_end(valist);
    lua_settop(_pLuaState, nTopIndex);
    return nResult;
}

void LuaScript::MainLoop(time_t tmTick)
{
    static uint32_t dwSumSecond = 0;
    static int32_t dwMem = 0;

    int32_t dwMemNow = lua_gc(_pLuaState, LUA_GCCOUNT, 0) * 1024 + lua_gc(_pLuaState, LUA_GCCOUNTB, 0);
    if (dwSumSecond % 300 == 0)
    {
        LOG_CUSTOM("lua", "======== lua memory = %d bytes ========", dwMemNow);
        dwSumSecond = 0;
    }
    else
    {
        if (dwMem * 2 < dwMemNow)
        {
            LOG_INFO("lua", "======== lua memory = %d bytes ========", dwMemNow);
        }
    }

    dwMem = dwMemNow;
    ++dwSumSecond;
}

void LuaScript::StartStepGC(int nData)
{
    lua_gc(_pLuaState, LUA_GCSTEP, nData);
}

bool LuaScript::RegObj(LuaItem* pOjb, const char* szClassName)
{
    if (!pOjb || !szClassName)
    {
        return false;
    }

    tolua_pushusertype(_pLuaState, pOjb, szClassName);
    int nIndex = luaL_ref(_pLuaState, LUA_REGISTRYINDEX);
    if (LUA_REFNIL == nIndex)
    {
        return false;
    }
    pOjb->SetKey(nIndex);
    return true;
}

void LuaScript::UnRefObj(LuaItem* pOjb)
{
    if (pOjb && _pLuaState && LUA_REFNIL != pOjb->GetKey())
        luaL_unref(_pLuaState, LUA_REGISTRYINDEX, pOjb->GetKey());
}

