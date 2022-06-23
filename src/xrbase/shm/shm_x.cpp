/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "shm_x.h"
#include <stdint.h>
#include "log/LogMgr.h"
#include "platform/file_func.h"
#include "string/str_format.h"
#include "const.h"

#ifdef _MSC_VER
#include <windows.h>
#else
#include <sys/shm.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>
#endif // _MSC_VER

namespace shm
{
    std::string shm_mk_key(const std::string& szFrom, const std::string& szTo)
    {
        return str_to_lower(DEF_MQ_SHM_HEAD + szFrom + "-" + szTo);
    }

    std::string shm_file_path()
    {
#ifdef _MSC_VER
        std::string strRet = file::get_app_data_path(DEF_APP_DATA_PATH);
        if (strRet.empty()) 
            strRet = file::get_local_path("/shm");
        else
            strRet += "shm";
        return strRet;
#else
        return "/dev/shm";
#endif // _MSC_VER

    }

    std::string shm_file_path(const std::string& szShmName)
    {
        std::string strPath = file::get_file_path(szShmName);
        std::string strFile = szShmName;
        if (strPath.empty())
        {
            strPath = shm_file_path();
            strFile = strPath + "/" + szShmName;
        }
        return strFile;
    }

    HANDLE_T shm_create_x(const std::string& szShmName, int64_t nSize, void** pAddr, int32_t& nHandle)
    {
        if (szShmName.empty())
        {
            LOG_ERROR("name is nullptr");
            return nullptr;
        }

        int32_t nRetCode = 0;
        HANDLE_T pHandle = nullptr;
        *pAddr = nullptr;
        nHandle = 0;

#if defined(_MSC_VER)   
        std::string strName = file::get_file_name(szShmName);
        std::string strPath = file::get_file_path(szShmName);
        if (strPath.empty()) strPath = shm_file_path();
        std::string strFile = shm_file_path(szShmName);
        file::create_dir(strPath);

        HANDLE_T hFile = CreateFile(strFile.c_str(), GENERIC_READ | GENERIC_WRITE,
            0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == NULL)
        {
            LOG_ERROR("create file error!");
            return 0;
        }

        pHandle = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, (DWORD) (nSize >> 32), (DWORD) nSize, strName.c_str());
        ERROR_LOG_EXIT0(pHandle);

        *pAddr = MapViewOfFile(pHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        ERROR_LOG_EXIT0(*pAddr != (void*) -1 && *pAddr != nullptr);
#else
        nHandle = shm_open(szShmName.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        ERROR_LOG_EXIT0(nHandle > 0);

        nRetCode = ftruncate(nHandle, nSize);
        ERROR_LOG_EXIT0(nRetCode == 0);

        *pAddr = mmap(nullptr, nSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, nHandle, 0);
        ERROR_LOG_EXIT0(*pAddr != (void*) -1 && *pAddr != nullptr);
        pHandle = *pAddr;
#endif
        LOG_CUSTOM("shm", "create success %s %lld", szShmName.c_str(), nSize);
        return pHandle;
Exit0:
#ifdef _MSC_VER
        if (pHandle)
            CloseHandle(pHandle);
#else
        if (nHandle)
            close(nHandle);
#endif
        * pAddr = nullptr;
        nHandle = 0;
        LOG_ERROR("shm_create error %s !", szShmName.c_str());
        return nullptr;
    }

    HANDLE_T shm_open_x(const std::string& szShmName, void** pAddr, int64_t& nSize, int32_t& nHandle)
    {
        if (szShmName.empty())
        {
            LOG_ERROR("name is nullptr");
            return nullptr;
        }

        int32_t nRetCode = 0;
        HANDLE_T pHandle = nullptr;
        struct stat st;
        *pAddr = nullptr;
        nHandle = -1;

#if defined(_MSC_VER)  
        std::string strFile = shm_file_path(szShmName);
        std::string strName = file::get_file_name(szShmName);

        ERROR_LOG_EXIT0(file::is_exsit_file(strFile));
        nRetCode = stat(strFile.c_str(), &st);
        ERROR_LOG_EXIT0(nRetCode == 0);
        nSize = st.st_size;

        pHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, strName.c_str());
        if (pHandle == nullptr)
        {
            HANDLE_T hFile = CreateFile(strFile.c_str(), GENERIC_READ | GENERIC_WRITE,
                0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == NULL)
            {
                LOG_ERROR("open file error!");
                return nullptr;
            }

            pHandle = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, (DWORD) (nSize >> 32), (DWORD) nSize, strName.c_str());
            ERROR_LOG_EXIT0(pHandle);
        }

        *pAddr = MapViewOfFile(pHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        ERROR_LOG_EXIT0(*pAddr != (void*) -1 && *pAddr != nullptr);

#else
        nHandle = shm_open(szShmName.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
        ERROR_LOG_EXIT0(nHandle > 0);

        nRetCode = fstat(nHandle, &st);
        ERROR_LOG_EXIT0(nRetCode == 0);
        nSize = st.st_size;

        *pAddr = mmap(pAddr, nSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, nHandle, 0);
        ERROR_LOG_EXIT0(*pAddr != (void*) -1 && *pAddr != nullptr);
        pHandle = *pAddr;
#endif // _MSC_VER
        LOG_CUSTOM("shm", "load success %s %lld", szShmName.c_str(), nSize);
        return pHandle;
Exit0:
#ifdef _MSC_VER
        if (pHandle)
            CloseHandle(pHandle);
#else
        if (nHandle)
            close(nHandle);
#endif
        * pAddr = nullptr;
        nHandle = -1;
        nSize = 0;
        LOG_ERROR("shm_open error %s !", szShmName.c_str());
        return nullptr;
    }

    HANDLE_T shm_open_readonly(const std::string& szShmName, void** pAddr, int64_t& nSize, int32_t& nHandle)
    {
        if (szShmName.empty())
        {
            LOG_ERROR("name is nullptr");
            return nullptr;
        }

        int32_t nRetCode = 0;
        HANDLE_T pHandle = nullptr;
        struct stat st;
        *pAddr = nullptr;
        nHandle = -1;

#if defined(_MSC_VER)   
        std::string strName = file::get_file_name(szShmName);
        std::string strFile = shm_file_path(szShmName);

        ERROR_LOG_EXIT0(file::is_exsit_file(strFile));
        nRetCode = stat(strFile.c_str(), &st);
        ERROR_LOG_EXIT0(nRetCode == 0);
        nSize = st.st_size;

        pHandle = OpenFileMapping(FILE_MAP_READ, 0, strName.c_str());
        if (pHandle == nullptr)
        {
            HANDLE_T hFile = CreateFile(strFile.c_str(), GENERIC_READ | GENERIC_WRITE,
                0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == NULL)
            {
                LOG_ERROR("open_readonly file error!");
                return nullptr;
            }

            pHandle = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, (DWORD) (nSize >> 32), (DWORD) nSize, strName.c_str());
            ERROR_LOG_EXIT0(pHandle);
        }

        *pAddr = MapViewOfFile(pHandle, FILE_MAP_READ, 0, 0, 0);
        ERROR_LOG_EXIT0(*pAddr != (void*) -1 && *pAddr != nullptr);
#else
        nHandle = shm_open(szShmName.c_str(), O_RDONLY, S_IRUSR | S_IWUSR);
        ERROR_LOG_EXIT0(nHandle > 0);

        nRetCode = fstat(nHandle, &st);
        ERROR_LOG_EXIT0(nRetCode == 0);
        nSize = st.st_size;

        *pAddr = mmap(pAddr, nSize, PROT_READ, MAP_SHARED | MAP_POPULATE, nHandle, 0);
        ERROR_LOG_EXIT0(*pAddr != (void*) -1 && *pAddr != nullptr);
        pHandle = *pAddr;
#endif // _MSC_VER

        LOG_CUSTOM("shm", "load success %s %lld", szShmName.c_str(), nSize);
        return pHandle;
Exit0:
#ifdef _MSC_VER
        if (pHandle)
            CloseHandle(pHandle);
#else
        if (nHandle)
            close(nHandle);
#endif
        * pAddr = nullptr;
        nHandle = -1;
        LOG_ERROR("shm_open_readonly error %s !", szShmName.c_str());
        return nullptr;
    }

    bool  shm_munmap(void* pAddr, int64_t nSize)
    {
        if (pAddr == nullptr)
        {
            LOG_ERROR("addr is nullptr");
            return false;
        }

        int32_t nRetCode = 0;
#if defined(_MSC_VER)
        nRetCode = UnmapViewOfFile(pAddr);
        ERROR_LOG_EXIT0(nRetCode);
#else
        nRetCode = munmap(pAddr, nSize);
        ERROR_LOG_EXIT0(nRetCode == 0);
#endif // _MSC_VER
        LOG_CUSTOM("shm", "unmap success %lld", nSize);

        return true;
Exit0:
        return false;
    }

    bool shm_close_handle(HANDLE_T handle, int32_t& nHandle)
    {
        if (handle == nullptr || nHandle == -1)
        {
            LOG_ERROR("handle is nullptr");
            return false;
        }

        int32_t nRetCode = 0;
#if defined(_MSC_VER)
        nRetCode = CloseHandle(handle);
        ERROR_LOG_EXIT0(nRetCode);
#else
        nRetCode = close(nHandle);
        ERROR_LOG_EXIT0(nRetCode == 0);
#endif // _MSC_VER
        LOG_CUSTOM("shm", "close handle success");
        return true;
Exit0:
        return false;
    }

    bool  shm_unlink_x(const std::string& szShmName)
    {
        if (szShmName.empty())
        {
            LOG_ERROR("name is nullptr");
            return false;
        }

        int32_t nRetCode = 0;
        int32_t nHandle = -1;
#if defined(_MSC_VER) 
        shm_delete_file(szShmName);

#else
		nHandle = shm_open(szShmName.c_str(), O_RDONLY, 0);
		if (nHandle == -1) return false;

        nRetCode = shm_unlink(szShmName.c_str());
        ERROR_EXIT0(nRetCode == 0);
#endif // _MSC_VER
        //LOG_CUSTOM("shm", "unlink success %s", szShmName.c_str());
        return true;
Exit0:
        LOG_ERROR("shm_unlink error %s !", szShmName.c_str());
        return false;
    }

    bool shm_find_x(const std::string& szShmName)
    {
        if (szShmName.empty()) return false;

        int32_t nHandle = -1;
        int32_t nRetCode = 0;
        HANDLE_T pHandle = nullptr;

#if defined(_MSC_VER) 
        std::string strPath = shm_file_path(szShmName);
        std::string strName = file::get_file_name(strPath);

        if (!file::is_exsit_file(strPath))
        {
            pHandle = OpenFileMapping(FILE_MAP_READ, 0, strName.c_str());
            if (pHandle == nullptr) return false;
            CloseHandle(pHandle);
        }

        LOG_CUSTOM("shm", "find success %s", szShmName.c_str());
        return true;
#else
        nHandle = shm_open(szShmName.c_str(), O_RDONLY, 0);
        if (nHandle == -1) return false;
        close(nHandle);
#endif
        LOG_CUSTOM("shm", "find success %s", szShmName.c_str());
        return true;
    }

    bool shm_destroy(const std::string& szShmName)
    {
        if (szShmName.empty())
        {
            LOG_ERROR("name is nullptr");
            return false;
        }

        int32_t nRetCode = 0;
        int32_t nHandle = -1;
        int32_t nSize = 0;
        void* pAddr = nullptr;
        std::string strName = file::get_file_name(szShmName);

#if defined(_MSC_VER) 
        shm_delete_file(szShmName);
#else
        {
            struct stat st;
            nHandle = shm_open(szShmName.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
            ERROR_LOG_EXIT0(nHandle != -1);

            nRetCode = fstat(nHandle, &st);
            ERROR_LOG_EXIT0(nRetCode == 0);
            nSize = st.st_size;

            pAddr = mmap(nullptr, nSize, PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_POPULATE, nHandle, 0);
            ERROR_LOG_EXIT0(pAddr);

            nRetCode = munmap(pAddr, nSize);
            CHECK_LOG_ERROR(nRetCode == 0);

            close(nHandle);

            nRetCode = shm_unlink(szShmName.c_str());
            ERROR_LOG_EXIT0(nRetCode == 0);
        }
#endif
        LOG_CUSTOM("shm", "destroy success %s", szShmName.c_str());
        return true;
Exit0:
        LOG_ERROR("shm_destroy error %s !", szShmName.c_str());

        return false;
    }

    bool shm_delete_file(const std::string& szShmName)
    {
        int32_t nRetCode = 0;
        int32_t nHandle = -1;
#if defined(_MSC_VER)
        std::string strPath = shm_file_path(szShmName);
        if (!file::is_exsit_file(strPath))
        {
            return true;
        }
        file::remove_file(strPath);
#else
		nHandle = shm_open(szShmName.c_str(), O_RDONLY, 0);
		if (nHandle == -1) return false;

		close(nHandle);
        nRetCode = shm_unlink(szShmName.c_str());
        ERROR_EXIT0(nRetCode == 0);
#endif
        LOG_CUSTOM("shm", "delete file success %s", szShmName.c_str());
        return true;
Exit0:
        LOG_ERROR("shm_delete_file error %s !", szShmName.c_str());
        return false;
    }
}