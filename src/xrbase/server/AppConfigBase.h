/*
 *
 *      Author: venture
 */
#pragma once

#include <string.h>
#include <string>
#include "dtype.h"
using namespace std;

namespace tinyxml2
{
	class XMLElement;
};

class AppConfigBase
{
private:
	std::string			_fileName;
	umap<string, string> _mapServer;

public:
	std::string     GetFileName() { return _fileName; }

	bool Load(const std::string& fileName);
	bool InitServer(tinyxml2::XMLElement* pRootElement);

	uint32_t ReadInt(const char* szServer, uint32_t nIndex, const char* szItem, const char* szAttr);
	bool ReadBool(const char* szServer, uint32_t nIndex, const char* szItem, const char* szAttr);
	int64_t ReadLong(const char* szServer, uint32_t nIndex, const char* szItem, const char* szAttr);
	string ReadStr(const char* szServer, uint32_t nIndex, const char* szItem, const char* szAttr);

	int64_t ReadAttrLong(const std::string& szAttr);
	uint32_t ReadAttrInt(const std::string& szAttr);

	bool ReadAttrBool(const std::string& szAttr);
	string ReadAttrStr(const std::string& szAttr);

private:
	void _ReadXmlEle(tinyxml2::XMLElement* pItem, string strItem);
};
