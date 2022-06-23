/*
 *
 *      Author: venture
 */
#include "TextTable.h"
#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "string/str_format.h"

#ifdef _MSC_VER
#include <io.h>
#else
#include <stdio.h>
#include "dirent.h"
#include <unistd.h>
#endif // _MSC_VER

using namespace std;
const char  FILE_LINE_END[2] = { 0x0d, 0x0a };

TextTable::TextTable(char cSeparator)
{
    _cSeparator = cSeparator;
    _pContent = nullptr;
    _pItems = nullptr;
    _pLines = nullptr;
    _bOpenMode = true;
    _InvalidLine.SetLine(this, -1);
}

TextTable::~TextTable()
{
    Clear();
}

// 载入一个文本表格文件 szFilename
bool TextTable::LoadTableFile(const char* szFilename)
{
    assert(szFilename != nullptr);

    Clear();

#ifdef _MSC_VER
    if (_access(szFilename, 0) == -1)
        return false;
#else
    if (access(szFilename, 0) == -1)
        return false;
#endif // _MSC_VER

    FILE* fp = fopen(szFilename, "rb");
    if (!fp)
        return false;
    fseek(fp, 0, SEEK_END);

    size_t iFileSize = ftell(fp);
    _iContentSize = iFileSize;
    _bOpenMode = true;

    rewind(fp);

    strcpy(_szFilename, szFilename);
    _pContent = (char*) malloc(iFileSize + 1);
    if (_pContent == nullptr) return false;

    fread(_pContent, 1, iFileSize, fp);
    _pContent[iFileSize] = 0;
    fclose(fp);

    if (!ParseTextTable())
    {
        Clear();
        return false;
    }

    _pItems = (char**) (malloc(sizeof(char*) * _iLineCount * _iColCount));
    if (_pItems == nullptr) return false;

    memset(_pItems, 0, sizeof(char*) * _iLineCount * _iColCount);

    _pLines = new TableLine[_iLineCount];
    for (int i = 0; i < _iLineCount; i++)
    {
        _pLines[i].SetLine(this, i);
    }

    ParseTextTable();

    return true;
}

//创建文件
bool TextTable::CreateTableFile(const char* szFilename, const int iLine, const int iCol)
{
    assert(szFilename != nullptr);
    assert(iLine > 0 && iCol > 0);

    Clear();

    FILE* fp = fopen(szFilename, "wb");
    if (!fp)
        return false;
    fclose(fp);

    strcpy(_szFilename, szFilename);
    _iLineCount = iLine;
    _iColCount = iCol;
    _pContent = (char*) malloc(1);
    if (_pContent == nullptr) return false;

    _iContentSize = 1;
    _bOpenMode = false;

    memset(_pContent, 0, 1);

    _pItems = (char**) (malloc(sizeof(char*) * _iLineCount * _iColCount));
    if (_pItems == nullptr) return false;
    memset(_pItems, 0, sizeof(char*) * _iLineCount * _iColCount);

    _pLines = new TableLine[_iLineCount];
    for (int i = 0; i < _iLineCount; i++)
    {
        _pLines[i].SetLine(this, i);
    }

    return true;
}

//保存数据
bool TextTable::Save()
{
    return SaveAs(_szFilename);
}

//另存数据
bool TextTable::SaveAs(const char* szFilename)
{
    assert(szFilename != nullptr);
    assert(_iLineCount > 0 && _iColCount > 0);

    char chL = '"';

    FILE* fp = fopen(szFilename, "wb");
    if (!fp)
        return false;
    for (int i = 0; i < _iLineCount; i++)
    {
        for (int j = 0; j < _iColCount; j++)
        {
            char* pItem = _pItems[i * _iColCount + j];
            if (pItem != nullptr)
            {
                string strSrc = pItem;
                string strTemp = strSrc;
                //strTemp = str_replace(strTemp, FILE_LINE_END, "\n");
                strTemp = str_replace(strTemp, "\"", "\"\"");

                size_t nLen = strTemp.length();
                if (strchr(pItem, _cSeparator) != nullptr
                    || strchr(pItem, '\n') != nullptr
                    || strchr(pItem, '"') != nullptr)
                {
                    fwrite(&chL, 1, 1, fp);
                    fwrite(strTemp.c_str(), 1, strTemp.length(), fp);
                    fwrite(&chL, 1, 1, fp);
                }
                else
                {
                    fwrite(pItem, 1, strlen(pItem), fp);
                }

                if (j == _iColCount - 1)
                {
                    fwrite(FILE_LINE_END, 1, 2, fp);
                }
                else
                {
                    fwrite(&_cSeparator, 1, 1, fp);
                }
            }
            else
            {
                if (j == _iColCount - 1)
                {
                    fwrite(FILE_LINE_END, 1, 2, fp);
                }
                else
                {
                    fwrite(&_cSeparator, 1, 1, fp);
                }
            }
        }
    }
    fclose(fp);
    return true;
}

// 清除所有信息
void TextTable::Clear()
{
    if (_pItems != nullptr)
    {
        if (!_bOpenMode)
        {
            for (int i = 0; i < _iLineCount * _iColCount; i++)
            {
                char* p = _pItems[i];
                if (p != nullptr)
                {
                    free(p);
                }
            }

        }

        free(_pItems);
        _pItems = nullptr;
    }

    if (_pLines != nullptr)
    {
        free(_pLines);
        _pLines = nullptr;
    }

    if (_pContent != nullptr)
    {
        free(_pContent);
        _pContent = nullptr;

    }
    _iTitleLine = 0;
    _iTitleCol = 0;
    _iColCount = 0;
    _iLineCount = 0;
    _iContentSize = 0;
    _iContentCur = 0;
    strcpy(_szFilename, "");
}

bool TextTable::ParseTextTable()
{
    assert(_pContent != nullptr);
    int iLine, iCol;
    int iState; // 0 普通, 1 字符串内
    char* pWord, * pCur, * pd;

    iState = 0;
    iLine = iCol = 0;
    pd = pWord = pCur = _pContent;
    while (*pCur)
    {
        if (iState == 0)
        {
            if (*pCur == '"')
            {
                iState = 1;
            }
            else if (*pCur == _cSeparator)
            {
                if (_pItems != nullptr)
                {
                    *pd = 0;
                    assert(iLine < _iLineCount&& iCol < _iColCount);
                    _pItems[iLine * _iColCount + iCol] = pWord;
                }
                iCol++;
                pd = pWord = pCur + 1;
                if (_pItems == 0)
                {
                    if (_iColCount < iCol) _iColCount = iCol;
                }
            }
            else if (*pCur == 0x0A || *pCur == 0x0D)
            {
                if (pCur[1] == 0x0A || pCur[1] == 0x0D)
                {
                    pCur++;
                }
                if (_pItems != nullptr)
                {
                    *pd = 0;
                    assert(iLine < _iLineCount&& iCol < _iColCount);
                    _pItems[iLine * _iColCount + iCol] = pWord;
                }
                iCol++;
                if (_pItems == nullptr)
                {
                    if (_iColCount < iCol) _iColCount = iCol;
                }
                iLine++;
                iCol = 0;
                pd = pWord = pCur + 1;
            }
            else
            {
                if (_pItems != nullptr)
                {
                    if (pCur != pd) *pd = *pCur;
                    pd++;
                }
            }
        }
        else if (iState == 1)
        {
            if (*pCur == '"')
            {
                if (pCur[1] == '"')
                {
                    // 还是双引号
                    pCur++;
                    if (_pItems)
                    {
                        if (pCur != pd) *pd = *pCur;
                        pd++;
                    }
                }
                else
                {
                    // 结束
                    iState = 0;
                }
            }
            else
            {
                if (_pItems != nullptr)
                {
                    if (pCur != pd) *pd = *pCur;
                    pd++;
                }
            }
        }
        pCur++;
    }
    if (pWord != pCur)
    {
        if (_pItems != nullptr)
        {
            *pd = 0;
            assert(iLine < _iLineCount&& iCol < _iColCount);
            _pItems[iLine * _iColCount + iCol] = pWord;
        }
        iCol++;
        if (_pItems == nullptr)
        {
            if (_iColCount < iCol) _iColCount = iCol;
        }
        iLine++;
    }

    _iLineCount = iLine;
    return true;
}

bool TextTable::FindPosByString(const char* szString, int& iLine, int& iCol)
{
    char* p;
    int i, j;
    for (i = 0; i < _iLineCount; i++)
    {
        for (j = 0; j < _iColCount; j++)
        {
            p = _pItems[i * _iColCount + j];
            if (p)
            {
                if (strcmp(p, szString) == 0)
                {
                    iLine = i;
                    iCol = j;
                    return true;
                }
            }
        }
    }
    return false;
}

int TextTable::FindLineByString(const char* szString)
{
    int iLine, iCol;
    if (FindPosByString(szString, iLine, iCol))
    {
        return iLine;
    }
    return -1;
}

int TextTable::FindColByString(const char* szString)
{
    int iLine, iCol;
    if (FindPosByString(szString, iLine, iCol))
    {
        return iCol;
    }
    return -1;
}

const TextTable::TableLine& TextTable::operator[](const char* szIdx) const
{
    for (int i = 0; i < _iLineCount; i++)
    {
        if (strcmp(_pItems[i * _iColCount + _iTitleCol], szIdx) == 0)
        {
            return _pLines[i];
        }
    }
    return _InvalidLine;
}

const char* TextTable::GetString(int iLine, const char* szColIdx) const
{
    char* pStr;
    static char szNull[] = "";
    if (iLine < 0) return szNull;
    for (int i = 0; i < _iColCount; i++)
    {
        if (strcmp(_pItems[_iTitleLine * _iColCount + i], szColIdx) == 0)
        {
            pStr = _pItems[iLine * _iColCount + i];
            if (pStr == nullptr) pStr = szNull;
            return pStr;
        }
    }
    return szNull;
}

const char* TextTable::GetString(const char* szLineIdx, const char* szColIdx) const
{
    return (*this)[szLineIdx][szColIdx];
}

// 取得指定行和列的字符串
const char* TextTable::GetString(int iLine, int iCol) const
{
    if (iLine < 0 || iLine >= _iLineCount || iCol < 0 || iCol >= _iColCount) return nullptr;
    if (_pItems[iLine * _iColCount + iCol] == nullptr) return "";
    return (const char*) _pItems[iLine * _iColCount + iCol];
}

bool TextTable::Char(int iLine, int iCol, char& vValue) const
{
    if (_pItems[iLine * _iColCount + iCol] == nullptr) return false;
    vValue = (char) atoi(_pItems[iLine * _iColCount + iCol]);
    return true;
}

bool TextTable::Byte(int iLine, int iCol, unsigned char& vValue) const
{
    if (_pItems[iLine * _iColCount + iCol] == nullptr) return false;
    vValue = (unsigned char) atoi(_pItems[iLine * _iColCount + iCol]);
    return true;
}

bool TextTable::Short(int iLine, int iCol, short& vValue) const
{
    if (_pItems[iLine * _iColCount + iCol] == nullptr) return false;
    vValue = (short) atoi(_pItems[iLine * _iColCount + iCol]);
    return true;
}

bool TextTable::Word(int iLine, int iCol, unsigned short& vValue) const
{
    if (_pItems[iLine * _iColCount + iCol] == nullptr) return false;
    vValue = (unsigned short) atoi(_pItems[iLine * _iColCount + iCol]);
    return true;
}

bool TextTable::Int(int iLine, int iCol, int& vValue) const
{
    if (_pItems[iLine * _iColCount + iCol] == nullptr) return false;
    vValue = (int) atoi(_pItems[iLine * _iColCount + iCol]);
    return true;
}

bool TextTable::UInt(int iLine, int iCol, unsigned int& vValue) const
{
    if (_pItems[iLine * _iColCount + iCol] == nullptr) return false;
    vValue = (unsigned int) atoi(_pItems[iLine * _iColCount + iCol]);
    return true;
}

bool TextTable::Long(int iLine, int iCol, long& vValue) const
{
    if (_pItems[iLine * _iColCount + iCol] == nullptr) return false;
    vValue = (long) atol(_pItems[iLine * _iColCount + iCol]);
    return true;
}

bool TextTable::DWord(int iLine, int iCol, unsigned long& vValue) const
{
    if (_pItems[iLine * _iColCount + iCol] == nullptr) return false;
    vValue = (unsigned long) atol(_pItems[iLine * _iColCount + iCol]);
    return true;
}

bool TextTable::Float(int iLine, int iCol, float& vValue) const
{
    if (_pItems[iLine * _iColCount + iCol] == nullptr) return false;
    vValue = (float) atof(_pItems[iLine * _iColCount + iCol]);
    return true;
}

bool TextTable::Double(int iLine, int iCol, double& vValue) const
{
    if (_pItems[iLine * _iColCount + iCol] == nullptr) return false;
    vValue = atof(_pItems[iLine * _iColCount + iCol]);
    return true;
}

bool TextTable::Int64(int iLine, int iCol, int64_t& vValue) const
{
    if (_pItems[iLine * _iColCount + iCol] == nullptr) return false;
    vValue = atoll(_pItems[iLine * _iColCount + iCol]);
    return true;
}

//填写数据
bool TextTable::WriteChar(int iLine, int iCol, const char            vValue)
{
    return WriteCellValue(iLine, iCol, &vValue, 1);
}
bool TextTable::WriteByte(int iLine, int iCol, const unsigned char   vValue)
{
    return WriteCellValue(iLine, iCol, (char*) &vValue, 1);
}

bool TextTable::WriteStr(int iLine, int iCol, const std::string& vValue)
{
    return WriteCellValue(iLine, iCol, vValue.c_str(), vValue.length());
}

bool TextTable::WriteTime(int iLine, int iCol, const time_t vValue)
{
    struct tm tmstruct;

#ifdef _MSC_VER
    localtime_s(&tmstruct, &vValue);
#else
    localtime_r(&vValue, &tmstruct);
#endif // _MSC_VER
    char ch[20];
    strftime(ch, 20, "%Y-%m-%d %H:%M:%S", &tmstruct);
    return WriteCellValue(iLine, iCol, ch, strlen(ch));
}

bool TextTable::WriteCellValue(int iLine, int iCol, const char* vValue, size_t nSize)
{
    assert(vValue != nullptr);
    assert(iLine < _iLineCount&& iCol < _iColCount);

    if (vValue == nullptr) return false;
    if (iLine >= _iLineCount || iCol >= _iColCount) return false;

    if (_pItems[iLine * _iColCount + iCol] != nullptr)
    {
        char* p = _pItems[iLine * _iColCount + iCol];
        free(p);
    }

    char* pnew = (char*) malloc(nSize + 1);
    if (pnew != nullptr)
    {
        memcpy(pnew, vValue, nSize);
        pnew[nSize] = 0;   
        _pItems[iLine * _iColCount + iCol] = pnew;
    }
    return true;
}
