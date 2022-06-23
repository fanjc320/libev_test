#pragma once
#include <iostream>
#include <string>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include "dtype.h"
using namespace std;

enum class EIniRes : int8_t
{
    SUCCESS = 0,
    ERROR,
    OPENFILE_ERROR,
    NO_ATTR
};

class IniOp
{
public:
    IniOp();

    virtual ~IniOp();

public:
    EIniRes GetStr(const std::string& mAttr, const std::string& cAttr, std::string& szValue);
    EIniRes GetInt(const std::string& mAttr, const std::string& cAttr, int32_t& iValue);
    EIniRes GetLong(const std::string& mAttr, const std::string& cAttr, int64_t& llValue);

    EIniRes OpenFile(const std::string& pathName);

    EIniRes CloseFile();

protected:
    EIniRes GetKey(const std::string& mAttr, const std::string& cAttr, std::string& szValue);

    FILE* m_fp = nullptr;

    typedef umap<std::string, std::string> KEYMAP;
    typedef umap<std::string, KEYMAP> MAINKEYMAP;
    MAINKEYMAP m_Map;
};

