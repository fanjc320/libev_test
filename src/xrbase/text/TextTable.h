/*
 *
 *      Author: venture
 */
#pragma once
#include <vector>
#include <string>
#include <stdio.h>
#include <stdint.h>

 /**
 * @file TextTable CSV文件操作
 */

 //写cvs表
 //不可以在写的同时读数据 venture
 /* void func()
 * {
 *      TextCsv tcsv;
 *      tcsv.CreateTableFile("c:\\text.csv", 3, 3);
 *      tcsv.WriteStr(0, 0, "ID");
 *      tcsv.WriteStr(0, 1, "NAME");
 *      tcsv.WriteStr(0, 2, "DESC");
 *
 *      tcsv.Write(1, 0, 1);
 *      tcsv.WriteStr(1, 1, "LI");
 *      tcsv.Write(1, 2, 98.5);
 *
 *      tcsv.Write(2, 0, 2);
 *      tcsv.WriteStr(2, 1, "WANG");
 *      tcsv.Write(2, 2, 97.6);
 *
 *      tcsv.Save();
 *
 *      printf("%s\t", tcsv[0][0].Str());
 *      printf("%s\t", tcsv[0][1].Str());
 *      printf("%s\t", tcsv[0][2].Str());
 *
 *      tcsv.SetTitleLine(0);
 *      tcsv.SetTitleCol(0);
 *
 *      printf("\n");
 *
 *      for (int i = 1; i < tcsv.GetLineCount(); i++)
 *      {
 *          printf("%d\t", tcsv[i][0].Int());
 *          printf("%s\t", tcsv[i][1].Str());
 *          printf("%.2f\t", tcsv[i][2].Float());
 *
 *          printf("\n");
 *      }
 *  }*/

 /** 表格读取程序的例子.
 *@code
 * void test_azoth_table()
 * {
 *     CTextCsv csv;
 *     csv.LoadTableFile("test.csv");
 *
 *     printf("Line: %d, Col: %d\n", csv.GetLineCount(), csv.GetMaxCol());
 *     int i, j;
 *     for (j=0; j<csv.GetLineCount(); j++)
 *     {
 *         for (i=0; i<csv.GetMaxCol(); i++)
 *         {
 *             printf("[%c]%s\t", csv[j][i].IsValid()?'T':'F', (const char*)csv[j][i]);
 *         }
 *         printf("\n");
 *     }
 *
 *     printf("1: %s\n", csv[2]["d1"]);
 *     printf("2: %s\n", csv["a3"][3]);
 *     printf("3: %s\n", csv["a3"]["d1"]);
 *     return;
 * }
 *    @endcode
 ------------------------------------------------------------------------*/

 /** 表格读取类.
 *   用于读取固定符合分割的文件，比如Excel导出的CSV文件
 */
class  TextTable
{
public:
    class  TableItem
    {
    public:
        TableItem(const char* szItemString) { _szItem = szItemString; }
        bool IsValid() const { return _szItem[0] != 0; }
        operator const       char* () const { return _szItem; }
        const char* Str() const { return _szItem; }
        const char* SafeStr()const { return _szItem ? _szItem : ""; }
        const bool           Bool() const { return                 atoi(_szItem) != 0; }
        const char           Char() const { return (char) _szItem[0]; }
        const unsigned char  Byte() const { return (unsigned char) atoi(_szItem); }
        const short          Short() const { return (short) atoi(_szItem); }
        const unsigned short Word() const { return (unsigned short) atoi(_szItem); }
        const int            Int() const { return                 atoi(_szItem); }
        const unsigned int   UInt() const { return (unsigned int) atoi(_szItem); }
        const long           Long() const { return                 atol(_szItem); }
        const unsigned long  DWord() const { return (unsigned long) atol(_szItem); }
        const float          Float() const { return (float) atof(_szItem); }
        const double         Double() const { return                 atof(_szItem); }
        const int64_t        Int64() const { return (int64_t) atoll(_szItem); }

    private:
        const char* _szItem;
    };
    class TableLine
    {
    protected:
        TableLine() { }
        void SetLine(TextTable* pTable, int iLineIdx) { _pTable = pTable; _iLineIdx = iLineIdx; }
    public:
        friend class TextTable;
        const TableItem operator[](int nIndex) const
        {
            return TableItem(_pTable->GetString(_iLineIdx, nIndex));
        }
        const TableItem operator[](const char* szIdx) const
        {
            return TableItem(_pTable->GetString(_iLineIdx, szIdx));
        }
    private:
        TextTable* _pTable = nullptr;
        int        _iLineIdx = 0;
    };

protected:
    TextTable(char cSeparator);
    virtual ~TextTable();
    bool ParseTextTable();

    bool WriteCellValue(int iLine, int iCol, const char* vValue, size_t nSize);

public:
    // 载入一个文本表格文件 szFilename
    bool LoadTableFile(const char* szFilename);
    //创建表格文件 venture
    bool CreateTableFile(const char* szFilename, const int iLine, const int iCol);
    //保存表格文件 venture
    bool Save();
    bool SaveAs(const char* szFilename);

    // 清除所有信息
    void Clear();

    // 是否已经载入
    bool IsLoaded() { return _pContent != nullptr; }

    const TableLine& operator[](int nIndex) const { return _pLines[nIndex]; }
    const TableLine& operator[](const char* szIdx) const;

    // 取得指定行和列的字符串
    const char* GetString(int iLine, int iCol) const;
    const char* GetString(int iLine, const char* szColIdx) const;
    const char* GetString(const char* szLineIdx, const char* szColIdx) const;

    bool Char(int iLine, int iCol, char& vValue) const;
    bool Byte(int iLine, int iCol, unsigned char& vValue) const;
    bool Short(int iLine, int iCol, short& vValue) const;
    bool Word(int iLine, int iCol, unsigned short& vValue) const;
    bool Int(int iLine, int iCol, int& vValue) const;
    bool UInt(int iLine, int iCol, unsigned int& vValue) const;
    bool Long(int iLine, int iCol, long& vValue) const;
    bool DWord(int iLine, int iCol, unsigned long& vValue) const;
    bool Float(int iLine, int iCol, float& vValue) const;
    bool Double(int iLine, int iCol, double& vValue) const;
    bool Int64(int iLine, int iCol, int64_t& vValue) const;

    //填写数据
    bool WriteChar(int iLine, int iCol, const char            vValue);
    bool WriteByte(int iLine, int iCol, const unsigned char   vValue);
    template <typename T>
    bool Write(int iLine, int iCol, const T vValue)
    {
        static_assert(std::is_integral<T>::value, "Type V must be intergral!");
        std::string strValue = std::to_string(vValue);
        return WriteStr(iLine, iCol, strValue);
    }
    bool WriteStr(int iLine, int iCol, const std::string& vValue);
    bool WriteTime(int iLine, int iCol, const time_t           vValue);

    int GetLineCount() { return _iLineCount; }   // 表格总的行数
    int GetColCount() { return _iColCount; }     // 表格的列数

    void SetTitleLine(int iIdx) { _iTitleLine = iIdx; } // 设置索引行(0 base)，用于用字符串索引表格
    int  GetTitleLine() { return _iTitleLine; }
    void SetTitleCol(int iIdx) { _iTitleCol = iIdx; }  // 设置索引列(0 base)，用于用字符串索引表格
    int  GetTitleCol() { return _iTitleCol; }

    // 查找关键字szString第一次出现的位置，找到返回true并且返回行列到iLine, iCol否则返回false
    bool FindPosByString(const char* szString, int& iLine, int& iCol);
    int FindLineByString(const char* szString); // 查找szString第一次出现的行，找不到返回-1
    int FindColByString(const char* szString);  // 查找szString第一次出现的列，找不到返回-1

protected:
    char        _cSeparator = 0;
    char	    _szFilename[1024] = { 0 };
    char*       _pContent = nullptr;
    char**      _pItems = nullptr;
    TableLine*  _pLines = nullptr;
    TableLine   _InvalidLine;

    int         _iLineCount = 0;
    int         _iColCount = 0;

    int         _iTitleLine = 0;
    int         _iTitleCol = 0;

    size_t      _iContentSize = 0;
    int         _iContentCur = 0;

    bool        _bOpenMode = 0;
};

/** 读取CSV文件.
*   用于读取Excel导出的CSV文件
*/
class  TextCsv : public TextTable
{
public:
    TextCsv() : TextTable(',') { }
    virtual ~TextCsv() { }
};