#include "pch.h"
#include "IniOp.h"
#include "string/str_format.h"
const size_t CONFIGLEN = 1024;

IniOp::IniOp()
{
    m_fp = nullptr;
}

IniOp::~IniOp()
{
    m_Map.clear();
    CloseFile();
}

EIniRes IniOp::OpenFile(const std::string& pathName)
{
    string szLine, szMainKey, szLastMainKey, szSubKey;
    char strLine[CONFIGLEN] = { 0 };
    KEYMAP mLastMap;
    size_t iIndexPos = -1;
    size_t iLeftPos = -1;
    size_t iRightPos = -1;
    m_fp = fopen(pathName.c_str(), "r");
    if (!m_fp)
    {
        printf("open ini file %s error!\n", pathName.c_str());
        return EIniRes::OPENFILE_ERROR;
    }
    m_Map.clear();

    while (fgets(strLine, CONFIGLEN, m_fp))
    {
        //cout << szLine << endl;
        szLine.assign(strLine);
        szLine = trim_left(szLine);

        iLeftPos = szLine.find("[");
        iRightPos = szLine.find("]");
        if (iLeftPos != string::npos && iRightPos != string::npos)
        {
            szLine.erase(iLeftPos, 1);
            iRightPos--;
            szLine.erase(iRightPos, 1);
            if(!szLastMainKey.empty())
                m_Map[szLastMainKey] = mLastMap;

            mLastMap.clear();  
            szLastMainKey = trim_s(szLine);
        }
        else
        {
            if (iIndexPos = szLine.find("="), string::npos != iIndexPos)
            {
                string szSubKey, szSubValue;
                szSubKey = szLine.substr(0, iIndexPos);
                szSubKey = trim_s(szSubKey);
                szSubValue = szLine.substr(iIndexPos + 1, szLine.length() - iIndexPos - 1);
                szSubValue = trim_s(szSubValue);
                //cout << "szSubValue:" << szSubValue << endl;
                mLastMap[szSubKey] = szSubValue;
            }
            else
            {
            }
        }
    }
    m_Map[szLastMainKey] = mLastMap;

    return EIniRes::SUCCESS;
}

EIniRes IniOp::CloseFile()
{
    if (m_fp != nullptr)
    {
        fclose(m_fp);
        m_fp = nullptr;
    }

    return EIniRes::SUCCESS;
}

EIniRes IniOp::GetKey(const std::string& mAttr, const std::string& cAttr, std::string& szValue)
{
    std::string szmAttr = trim_s(mAttr);
    std::string szcAttr = trim_s(cAttr);
    auto itrAttr = m_Map.find(mAttr);
    if (itrAttr == m_Map.end()) return EIniRes::NO_ATTR;

    KEYMAP& mKey = itrAttr->second;
    auto itr = mKey.find(cAttr);
    if( itr != mKey.end())
    {
        szValue = itr->second;
        return EIniRes::SUCCESS;
    }
    return EIniRes::NO_ATTR;
}

EIniRes IniOp::GetStr(const std::string& mAttr, const std::string& cAttr, std::string& szValue)
{
    if (EIniRes::SUCCESS != GetKey(mAttr, cAttr, szValue))
    {
        szValue = "nullptr";
        return EIniRes::ERROR;
    }
    return EIniRes::SUCCESS;
}

EIniRes IniOp::GetInt(const std::string& mAttr, const std::string& cAttr, int32_t& iValue)
{
    std::string szKey;
    if (EIniRes::SUCCESS == GetKey(mAttr, cAttr, szKey))
    {
        iValue = std::atoi(szKey.c_str());
        return EIniRes::SUCCESS;
    }
    iValue = -1;
    return EIniRes::ERROR;
}

EIniRes IniOp::GetLong(const std::string& mAttr, const std::string& cAttr, int64_t& llValue)
{
    std::string szKey;
    if (EIniRes::SUCCESS == GetKey(mAttr, cAttr, szKey))
    {
        llValue = std::atoll(szKey.c_str());
        return EIniRes::SUCCESS;
    }
    llValue = -1LL;
    return EIniRes::ERROR;
}