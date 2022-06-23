/*
 *
 *      Author: venture
 */
#include "dtype.h"
#include "LoadConfigScript.h"
#include "tolua++.h"
#include "LoadConfigMgr.h"
#include "log/LogMgr.h"
#include "platform/file_func.h"
#include "script/LuaScript.h"
#include "string/str_split.h"
#include "LoadConfigCsv.h"
#include "const.h"

static LuaScript g_script;
bool LoadConfigScript::Init()
{
    bool ret;

    std::string strScript = file::get_local_path(DEF_SCRIPT_PATH) + "game_data";
    ret = g_script.Init(strScript);

    if (!ret)
    {
        LOG_ERROR("error script init csv %s", strScript.c_str());
        return false;
    }

    return true;
Exit0:
    LOG_ERROR("Config Init error!!!");
    return false;
}

bool LoadConfigScript::LoadFromFile(game_data* pItem, const char* objName
    , const char* type, const char* name, const char* value)
{
    return _load_from_file(pItem, objName, type, name, value);
}

LuaScript* LoadConfigScript::GetScript()
{
    return &g_script;
}

bool LoadConfigScript::RunToLuaFunc(TO_LUA_FUNC func)
{
    if (g_script.GetLuaState() == nullptr) return false;
    return func(g_script.GetLuaState());
}

bool LoadConfigScript::_load_from_file(game_data* pItem, const char* objName, const char* type, const char* name, const char* value)
{
    bool ret = false;
    std::string strName = name;
    size_t left = strName.find('[');
    size_t right = strName.rfind(']');
    if (left != std::string::npos && right != std::string::npos)
    {
        std::string strNum = strName.substr(left + 1, right - left - 1);
        int32_t Num = atoi(strNum.c_str());
        strName = strName.substr(0, left);
        std::vector<std::string> vecValue;
        str_split(value, ',', vecValue, true, true);
        for (int32_t i = 0; i < vecValue.size() && i < Num; ++i)
        {
            ret = _load_from_file(pItem, objName, type, strName.c_str(), i, vecValue[i].c_str());
            if (!ret) return false;
        }
        return true;
    }

    int nRet = 0;
    if (strcmp_ncase(type, "int") == 0 || strcmp_ncase(type, "int32_t") == 0 ||
        strcmp_ncase(type, "int32") == 0 || strcmp_ncase(type, "bool") == 0)
    {
        int32_t val = std::atoi(value);
        nRet = g_script.CallFunc("game_data_lua", "read_number", "sio>", name, val, pItem, objName);
    }
    else if (strcmp_ncase(type, "long long") == 0 or strcmp_ncase(type, "int64_t") == 0 ||
        strcmp_ncase(type, "int64") == 0)
    {
        int64_t val = std::atoll(value);
        nRet = g_script.CallFunc("game_data_lua", "read_number", "slo>", name, val, pItem, objName);
    }
    else if (strcmp_ncase(type, "double") == 0 or strcmp_ncase(type, "float") == 0)
    {
        double val = std::atof(value);
        nRet = g_script.CallFunc("game_data_lua", "read_number", "sfo>", name, val, pItem, objName);
    }
    else if (strcmp_ncase(type, "string") == 0)
    {
        nRet = g_script.CallFunc("game_data_lua", "read_string", "sso>", name, value, pItem, objName);
    }

Exit0:
    if (nRet != 1)
    {
        LOG_ERROR("_load_from_file %s %s %s %s", objName, type, name, value);
    }
    return nRet == 1;
}

bool LoadConfigScript::_load_from_file(game_data* pItem, const char* objName, const char* type, const char* name, int32_t index, const char* value)
{
    int ret = 0;
    if (strcmp_ncase(type, "int") == 0 || strcmp_ncase(type, "int32_t") == 0 ||
        strcmp_ncase(type, "int32") == 0 || strcmp_ncase(type, "bool") == 0)
    {
        int32_t val = std::atoi(value);
        ret = g_script.CallFunc("game_data_lua", "read_number_array", "siio>", name, index, val, pItem, objName);
    }
    else if (strcmp_ncase(type, "long long") == 0 or strcmp_ncase(type, "int64_t") == 0 ||
        strcmp_ncase(type, "int64") == 0)
    {
        int64_t val = std::atoll(value);
        ret = g_script.CallFunc("game_data_lua", "read_number_array", "silo>", name, index, val, pItem, objName);
    }
    else if (strcmp_ncase(type, "double") == 0 or strcmp_ncase(type, "float") == 0)
    {
        double val = std::atof(value);
        ret = g_script.CallFunc("game_data_lua", "read_number_array", "sifo>", name, index, val, pItem, objName);
    }
    else if (strcmp_ncase(type, "string") == 0)
    {
        ret = g_script.CallFunc("game_data_lua", "read_string_array", "siso>", name, index, value, pItem, objName);
    }
    if (ret != 1)
    {
        LOG_ERROR("_load_from_file %s %s %s %d %s", objName, type, name, index, value);
    }
    return ret;
}
