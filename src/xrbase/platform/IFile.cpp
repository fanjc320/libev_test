/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "IFile.h"

#include <sys/stat.h>
#include <stdio.h>

#if defined(_MSC_VER)
#include <windows.h>
#else
#include <stdarg.h>
#include<sys/types.h>
#include<unistd.h>
#include <dirent.h>
#include <sys/sysinfo.h>
#endif	//_MSC_VER

#include "log/LogMgr.h"
#include "platform/file_func.h"
namespace file
{
    class InerFile : public IFile
    {
    public:
        virtual bool Close(void) = 0;
    };

    class GenFile : public InerFile
    {
    public:
        GenFile();
        virtual ~GenFile();

        bool Init(const char* pcszFileName, const char* pcszMode);
        bool Uninit(void);

        virtual bool Close(void);

        virtual size_t Read(void* pData, size_t dwLen);
        size_t Write(const void* pData, size_t dwLen);
        virtual bool  Seek(size_t nOffset, size_t nOrigin);
        virtual bool Eof(void);
        virtual size_t Size(void);

    private:
        FILE* _pFile;
    };

    GenFile::GenFile()
    {
        _pFile = nullptr;
    }

    GenFile::~GenFile()
    {
        Uninit();
    }

    bool GenFile::Init(const char* pcszFileName, const char* pcszMode)
    {
        ERROR_LOG_EXIT0(_pFile == nullptr);

        _pFile = fopen(pcszFileName, pcszMode);
        ERROR_LOG_EXIT0(_pFile);

        return true;
Exit0:
        return false;
    }

    bool GenFile::Uninit(void)
    {
        if (_pFile)
        {
            fclose(_pFile);
        }
        _pFile = nullptr;

        return true;
    }

    bool GenFile::Close(void)
    {
        Uninit();
        delete this;

        return true;
    }

    size_t GenFile::Read(void* pData, size_t dwLen)
    {
        return fread(pData, 1, dwLen, _pFile);
    }

    size_t GenFile::Write(const void* pData, size_t dwLen)
    {
        return fwrite(pData, 1, dwLen, _pFile);
    }

    bool GenFile::Seek(size_t nOffset, size_t nOrigin)
    {
        return fseek(_pFile, (long) nOffset, (int) nOrigin) == 0;
    }

    bool GenFile::Eof(void)
    {
        if (_pFile)
            return feof(_pFile);
        return true;
    }

    size_t GenFile::Size(void)
    {
        size_t dwResult = 0;
        size_t nRetCode = 0;
        size_t nCurrent = 0;

        ERROR_LOG_EXIT0(_pFile);

        nCurrent = ftell(_pFile);

        nRetCode = fseek(_pFile, 0, SEEK_END);
        ERROR_LOG_EXIT0(nRetCode == 0);

        dwResult = ftell(_pFile);

        nRetCode = fseek(_pFile, (long) nCurrent, SEEK_SET);
        ERROR_LOG_EXIT0(nRetCode == 0);

Exit0:
        return dwResult;
    }

    /************************************************************************/

    IFile* open_file(const char* pcszFileName, const char* pcszMode)
    {
        int32_t nRetCode = 0;
        IFile* pResult = nullptr;

        pResult = new GenFile;
        ERROR_LOG_EXIT0(pResult);

        nRetCode = ((GenFile*) pResult)->Init(pcszFileName, pcszMode);
        ERROR_LOG_EXIT0(nRetCode);

        return pResult;
Exit0:
        SAFE_DELETE(pResult);

        return nullptr;
    }

    bool close_file(IFile* pFile)
    {
        ERROR_LOG_EXIT0(pFile);

        ((InerFile*) pFile)->Close();

        return true;
Exit0:
        return false;
    }

    time_t get_file_create_time(const char* pcszFileName)
    {
        time_t tmRet = 0;
        int32_t nRetCode = 0;
        FILE* pFile = nullptr;

        ERROR_LOG_EXIT0(pcszFileName);

        pFile = fopen(pcszFileName, "rb");
        if (pFile)
        {
            int fd;
            struct stat FileStat;

            fd = FILENO(pFile);
            ERROR_LOG_EXIT0(fd != -1);

            nRetCode = fstat(fd, &FileStat);
            ERROR_LOG_EXIT0(nRetCode == 0);

            tmRet = FileStat.st_ctime;
        }

Exit0:
        if (pFile)
            fclose(pFile);

        return tmRet;
    }

    time_t get_file_modify_time(const char* pcszFileName)
    {
        time_t tmRet = 0;
        int32_t nRetCode = 0;
        FILE* pFile = nullptr;

        ERROR_LOG_EXIT0(pcszFileName);

        pFile = fopen(pcszFileName, "rb");
        if (pFile)
        {
            int fd;
            struct stat FileStat;

            fd = FILENO(pFile);
            ERROR_LOG_EXIT0(fd != -1);

            nRetCode = fstat(fd, &FileStat);
            ERROR_LOG_EXIT0(nRetCode == 0);

            tmRet = FileStat.st_mtime;
        }

Exit0:
        if (pFile)
            fclose(pFile);

        return tmRet;
    }
}
