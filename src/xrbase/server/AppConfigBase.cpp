/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <string.h>
#include <string>
#include "tinyxml2.h"
#include "log/LogMgr.h"
#include "string/str_num.h"
#include "server/AppConfigBase.h"
#include "platform/file_func.h"

using namespace tinyxml2;
bool AppConfigBase::Load(const std::string& fileName)
{
    _fileName = fileName;
    //创建一个XML的文档对象。
    XMLDocument eDocument;
    if (eDocument.LoadFile(_fileName.c_str()) != XML_SUCCESS)
    {
        LOG_ERROR("Xmlfile NoExsit:%s", _fileName.c_str());
        return false;
    }
   
    //获得根元素，即ServerList。
    XMLElement* pRootElement = eDocument.RootElement();
    if (pRootElement == nullptr)
    {
        LOG_ERROR("Xmlfile root error:%s", _fileName.c_str());
        return false;
    }

    return InitServer(pRootElement);
}

bool AppConfigBase::InitServer(XMLElement* pRootElement)
{
    //获得第一个ServerList节点
    _mapServer.clear();

    _ReadXmlEle(pRootElement, "");

    string strKey;
    XMLElement* pFirstList = pRootElement->FirstChildElement();
    while (pFirstList != nullptr)
    {
        strKey = pFirstList->Value();
        _ReadXmlEle(pFirstList, strKey);

        //输出接点名Server
        XMLElement* pFirstServer = pFirstList->FirstChildElement();
        while (pFirstServer != nullptr)
        {
            auto itrAttr = pFirstServer->Attribute("ID");
            if (itrAttr == nullptr)
            {
                itrAttr = pFirstServer->Value();
            }

            if (itrAttr != nullptr)
            {
                string strServer = strKey;
                strServer = strServer + ":";
                strServer = strServer + itrAttr;

                string strIndex = strServer + ":";
                strIndex = strIndex + pFirstServer->Value();
                _ReadXmlEle(pFirstServer, strIndex);

                XMLElement* pItem = pFirstServer->FirstChildElement();
                while (pItem != nullptr)
                {
                    string strItem = strServer;
                    strItem = strItem + ":";
                    strItem = strItem + pItem->Value();
                    _ReadXmlEle(pItem, strItem);
                    pItem = pItem->NextSiblingElement();
                }
            }

            pFirstServer = pFirstServer->NextSiblingElement();
        }

        pFirstList = pFirstList->NextSiblingElement();
    }

    return true;
}

void AppConfigBase::_ReadXmlEle(XMLElement* pItem, string strItem)
{
    const XMLAttribute* pIDAttSub = pItem->FirstAttribute();
    while (pIDAttSub != nullptr)
    {
        string strIndex = strItem;
        if (!strItem.empty())
            strIndex = strIndex + ":";

        strIndex = strIndex + pIDAttSub->Name();
        _mapServer[strIndex] = pIDAttSub->Value();
        pIDAttSub = pIDAttSub->Next();
    }
}

uint32_t AppConfigBase::ReadInt(const char* szServer, uint32_t nIndex, const char* szItem, const char* szAttr)
{
    return (uint32_t) ReadLong(szServer, nIndex, szItem, szAttr);
}

bool AppConfigBase::ReadBool(const char* szServer, uint32_t nIndex, const char* szItem, const char* szAttr)
{
    return ReadLong(szServer, nIndex, szItem, szAttr) == 1;
}

int64_t AppConfigBase::ReadLong(const char* szServer, uint32_t nIndex, const char* szItem, const char* szAttr)
{
    int64_t nValue = 0;
    string strValue = ReadStr(szServer, nIndex, szItem, szAttr);
    if (strValue.empty()) return 0;

    nValue = stoll_x(strValue);
    return nValue;
}

string AppConfigBase::ReadStr(const char* szServer, uint32_t nIndex, const char* szItem, const char* szAttr)
{
    std::string ss;
    if (szItem == nullptr || strlen(szItem) == 0)
        ss = ss + szServer + ":" + std::to_string(nIndex) + ":" + "0" + ":" + szAttr;
    else
        ss = ss + szServer + ":" + std::to_string(nIndex) + ":" + szItem + ":" + szAttr;
    string strValue = "";

    auto itServer = _mapServer.find(ss);
    if (itServer != _mapServer.end())
    {
        strValue = itServer->second;
    }
    else
    {
        LOG_WARNING("xml no value[ %s-%d-%s-%s ]", szServer, nIndex, szItem, szAttr);
    }
    return strValue;
}

int64_t AppConfigBase::ReadAttrLong(const std::string& szAttr)
{
    int64_t nValue = 0;
    string strValue = ReadAttrStr(szAttr);
    if (strValue.empty()) return 0;

    nValue = stoll_x(strValue);
    return nValue;
}

uint32_t AppConfigBase::ReadAttrInt(const std::string& szAttr)
{
    return (uint32_t) ReadAttrLong(szAttr);
}

bool AppConfigBase::ReadAttrBool(const std::string& szAttr)
{
    return ReadAttrInt(szAttr) == 1;
}

string AppConfigBase::ReadAttrStr(const std::string& szAttr)
{
    string strValue = "";
    auto itr = _mapServer.find(szAttr);
    if (itr != _mapServer.end())
    {
        strValue = itr->second;
    }
    else
    {
        LOG_WARNING("xml no value[ %s ]", szAttr.c_str());
    }
    return strValue;
}