/*
 *
 *      Author: venture
 */
#pragma once
#include "dtype.h"
#include "text/TextTable.h"
#include "log/LogMgr.h"
#include "script/LuaScript.h"
#include "platform/file_func.h"
#include "LoadConfigBase.h"
#include "LoadConfigMgr.h"
#include "LoadConfigScript.h"
#include "const.h"

#define MAKE_LOAD_FILE_PATH(T)  (file::get_local_path(DEF_GAME_DATA_PATH) + #T + ".csv")

#define INIT_LOAD_FILE_PATH(T, PATH) LoadConfigCsv<T>::Instance().Init((PATH), (#T));
#define INIT_LOAD_FILE(T) LoadConfigCsv<T>::Instance().Init((MAKE_LOAD_FILE_PATH(T)), (#T));

class LuaScript;
template <class T>
class LoadConfigCsv : public LoadConfigBase
{
public:
    static LoadConfigCsv<T>& Instance(void);
    virtual bool Init(const std::string& szPath, const std::string& objName) override;
    virtual bool Unit() override;
    virtual bool Load() override;
    virtual bool LoadEnd() override;

private:
    static LoadConfigCsv<T> _Instance;
};

template <class T>
bool LoadConfigCsv<T>::LoadEnd()
{
    return LoadConfigBase::LoadEnd();
}

template <class T>
bool LoadConfigCsv<T>::Unit()
{
    return LoadConfigBase::Unit();
}

template <class T>
bool LoadConfigCsv<T>::Load()
{
    TextCsv csv;
    int ret = csv.LoadTableFile(_szPath.c_str());
    if (!ret)
    {
        LOG_ERROR("error load csv %s", _szPath.c_str());
        return false;
    }

    const char* type, * name, * comment, * value;
    for (int i = 3; i < csv.GetLineCount(); ++i)
    {
        T* pNew = nullptr;
        name = csv.GetString(0, 0);
        if (strcmp_ncase(name, "id") == 0 || strcmp_ncase(name, "ID") == 0)
        {
            auto itr = _mapVal.find(::atoi(name));
            if (itr != _mapVal.end())
                pNew = (T*)(itr->second);
            else
                pNew = new T();
        }
        else
        {
            LOG_ERROR("ID Col Must exsit csv %s", _szPath);
            return false;
        }

        for (int j = 0; j < csv.GetColCount(); ++j)
        {
            name = csv.GetString(0, j);
            type = csv.GetString(1, j);
            comment = csv.GetString(2, j);
            value = csv.GetString(i, j);
            ret = LoadConfigScript::LoadFromFile(pNew, _objName.c_str(), type, name, value);
        }
        ret = pNew->read(pNew->id);
        if (!ret)
        {
            LOG_ERROR("Load Error! %s, row: %d", _szPath.c_str(), i);
            return false;
        }

        pNew->key = pNew->id;
        _mapVal[pNew->key] = pNew;
    }

    return true;
}

template<class T>
LoadConfigCsv<T> LoadConfigCsv<T>::_Instance;

template <class T>
inline bool LoadConfigCsv<T>::Init(const std::string& szPath, const std::string& objName)
{
    if (!file::is_exsit_file(szPath))
    {
        LOG_ERROR("not exsit csv %s", szPath.c_str());
        return false;
    }
    _szPath = szPath;
    _objName = objName;
    LoadConfigMgr::GetInstance()->AddLoadConfigBase(this);
    return true;
}


template<class T>
inline LoadConfigCsv<T>& LoadConfigCsv<T>::Instance(void)
{
    return _Instance;
}

