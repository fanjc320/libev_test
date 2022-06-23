/*
 *
 *      Author: venture
 */
#include "pch.h"

#include <stdio.h>
#ifndef _MSC_VER
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#else
#include <windows.h>
#include <process.h>
#include <stdint.h>
#include <io.h>
#include <shlobj.h>

#endif // _MSC_VER

#include "file_func.h"
#include "string/str_format.h"
#include "string/str_num.h"

namespace file
{

    std::string get_app_data_path(const string& strName)
    {
        std::string strRet;
#ifdef _MSC_VER
        TCHAR MyDir[_MAX_PATH];
        SHGetSpecialFolderPath(NULL, MyDir, CSIDL_APPDATA, 0);
        strRet = MyDir;
        strRet += "/" + strName;
        return strRet;
#else
        strRet = "~/.";
        strRet += strName;
        return strRet;
#endif // _MSC_VER
    }

    bool create_dir(const std::string& src)
    {
        char sz[1024];
        memcpy_s(sz, 1024, src.c_str(), src.length());

        size_t i = 0;
        int iRet = 0;
        size_t iLen = src.length();
        if (sz[iLen - 1] != '\\' && sz[iLen - 1] != '/')
        {
            sz[iLen] = '/';
            sz[iLen + 1] = '\0';
        }
        for (i = 1; i < iLen + 1; ++i)
        {
            if (sz[i] == '\\' || sz[i] == '/')
            {
                sz[i] = '\0';
                iRet = ACCESS(sz, 0);
                if (iRet != 0)
                {
                    iRet = MKDIR(sz);
                    if (iRet != 0)
                    {
                        return false;
                    }
                }
                sz[i] = '/';
            }
        }
        return true;
    }

    std::string get_full_name()
    {
        static std::string strRet;
        if (!strRet.empty())
            return strRet;

        char szpath[1024];

#ifndef _MSC_VER
        char szlink[1024];
        sprintf(szlink, "/proc/%d/exe", getpid());
        int32_t rslt = readlink(szlink, szpath, sizeof(szpath));
        if (rslt < 0) return "";
        szpath[rslt] = '\0';
        strRet.append(szpath, rslt);
#else
        uint32_t len = GetModuleFileName(nullptr, szpath, sizeof(szpath) - 1);
        strRet.append(szpath, len);
#endif

        return strRet;
    }

    std::string get_exec_path()
    {
        static std::string strRet;
        if (!strRet.empty())
            return strRet;

        std::string strTemp;
        strTemp = get_full_name();
        const char* szpath = strTemp.c_str();

        const char* path_end = nullptr;
#ifndef _MSC_VER
        path_end = strrchr(szpath, '/');
#else
        path_end = strrchr(szpath, '\\');
#endif // !_MSC_VER


        if (path_end == nullptr) return "";
        strRet.append(szpath, (path_end - szpath));

        return strRet;
    }

    std::string get_local_path(const std::string& szFile)
    {
        std::string ret = get_exec_path();
        ret = ret + "/" + szFile;
        return ret;
    }

    std::string get_exec_name(bool delDebugFlag)
    {
        static std::string strRet;
        if (!strRet.empty())
            return strRet;

        std::string strTemp, strFullPath;
        strTemp = get_full_name();

        const char* szpath = strTemp.c_str();
        const char* path_end = nullptr;
#ifndef _MSC_VER
        path_end = strrchr(szpath, '/');
#else
        path_end = strrchr(szpath, '\\');
#endif // !_MSC_VER

        if (path_end == nullptr) return "";
        strFullPath.append(path_end + 1, strlen(path_end + 1));

#ifdef _MSC_VER
        int nPos = (int) strFullPath.find_last_of('.');
        if (nPos != string::npos)
        {
            strRet = strFullPath.substr(0, nPos);
        }
        else
            strRet = strFullPath;
#else
        strRet = strFullPath;
#endif
        if (delDebugFlag)
        {
            std::string strFlag = strRet.substr(strRet.length() - 2, 2);
            if (strFlag.empty() || strFlag != "_d" || strFlag != "_D") return strRet;
            strRet = strRet.substr(0, strRet.length() - 2);
        }
        return strRet;
    }
    bool get_folder_file(const std::string& szDir, std::vector<std::string>& vecFile
        , bool bRecurse, const std::string& szExt)
    {
        if (szDir.empty())
            return false;

        std::string strName;
        unsigned  bDir = 0;

        char szPath[1024] = {};

#ifdef _MSC_VER
        sprintf_safe(szPath, "%s\\*", szDir.c_str());

        _finddata_t info;
        intptr_t handle = ::_findfirst(szPath, &info);
        if (handle < 0)
        {
            LOG_ERROR("get_folder_list error! %s", szDir.c_str());
            return false;
        }
        do
        {
            strName = info.name;
            bDir = info.attrib & _A_SUBDIR;

            if (strName == "." || strName == ".." || strName.empty())
                continue;

            std::string strFilePath = szDir;
            strFilePath += "/";
            strFilePath += strName;

            if (bDir)
            {
                if (bRecurse) get_folder_file(strFilePath, vecFile, bRecurse, szExt);
            }
            else
            {
                if (!szExt.empty())
                {
                    size_t nPos = strName.rfind('.');
                    if (nPos != string::npos)
                    {
                        string strExt = strName.substr(nPos + 1, strName.size() - nPos - 1);
                        if (strcmp_ncase(strExt, szExt) != 0)
                            continue;

                        vecFile.push_back(strFilePath);
                    }
                }
                else
                    vecFile.push_back(strFilePath);
            }
        } while (_findnext(handle, &info) == 0);

        ::_findclose(handle);
        return true;
#else
        struct dirent* ent = NULL;
        DIR* pDir = opendir(szDir.c_str());
        while (pDir != NULL && (ent = readdir(pDir)))
        {
            strName.clear();
            strName.append(ent->d_name, strlen(ent->d_name));
            bDir = ent->d_type & DT_DIR;

            if (strName == "." || strName == "..")
                continue;

            std::string strFilePath = szDir;
            strFilePath += "/";
            strFilePath += strName;

            if (bDir)
            {
                if (bRecurse) get_folder_file(strFilePath, vecFile, bRecurse, szExt);
            }
            else
            {
                if (!szExt.empty())
                {
                    size_t nPos = strName.rfind('.');
                    if (nPos != string::npos)
                    {
                        string strExt = strName.substr(nPos + 1, strName.size() - nPos - 1);
                        if (strcmp_ncase(strExt, szExt) != 0)
                            continue;

                        vecFile.push_back(strFilePath);
                    }
                }
                else
                    vecFile.push_back(strFilePath);
            }

        }
        if (pDir != nullptr)
            closedir(pDir);

        return true;
#endif // _MSC_VER
    }
    bool get_folder_list(const std::string& szDir, std::vector<std::string>& vecFolder
        , bool bRecurse)
    {
        if (szDir.empty())
            return false;

        std::string strName;
        unsigned  bDir = 0;
        char szPath[1024] = {};

#ifdef _MSC_VER
        sprintf_safe(szPath, "%s\\*", szDir.c_str());

        _finddata_t info;
        intptr_t handle = ::_findfirst(szPath, &info);
        if (handle == -1)
        {
            LOG_ERROR("get_folder_list error! %s", szDir.c_str());
            return false;
        }
        do
        {
            strName = info.name;
            bDir = info.attrib & _A_SUBDIR;

            if (strName == "." || strName == ".." || strName.empty())
                continue;

            std::string strFilePath = szDir;
            strFilePath += "/";
            strFilePath += strName;

            if (bDir)
            {
                if (bRecurse) get_folder_list(strFilePath, vecFolder, bRecurse);
                vecFolder.push_back(strFilePath);
            }
            else
                continue;
        } while (_findnext(handle, &info) == 0);
        ::_findclose(handle);
        return true;
#else
        struct dirent* ent = NULL;
        DIR* pDir = opendir(szDir.c_str());
        while (pDir != NULL && (ent = readdir(pDir)))
        {
            strName.clear();
            strName.append(ent->d_name, strlen(ent->d_name));
            bDir = ent->d_type & DT_DIR;

            if (strName == "." || strName == "..")
                continue;

            std::string strFilePath = szDir;
            strFilePath += "/";
            strFilePath += strName;

            if (bDir)
            {
                if (bRecurse) get_folder_list(strFilePath, vecFolder, bRecurse);
                vecFolder.push_back(strFilePath);
            }
            else
                continue;
        }
        if (pDir != nullptr)
            closedir(pDir);
        return true;
#endif

    }
    bool remove_file(const std::string& filename)
    {
        return remove(filename.c_str()) == 0;
    }

    bool remove_dir(const std::string& folder)
    {
        if (!remove_dir_file(folder)) return false;
        return RMDIR(folder.c_str()) == 0;
    }

    bool remove_dir_file(const std::string& folder)
    {
        if (folder.size() == 0)
            return false;

        char szPath[1024] = { 0 };
        sprintf_safe(szPath, "%s\\*", folder.c_str());

        std::string strName;
        unsigned  bDir = 0;

#ifdef _MSC_VER
        _finddata_t info;
        intptr_t handle = _findfirst(szPath, &info);
        if (handle < 0)
            return false;
        do
        {
            strName = info.name;
            bDir = info.attrib & _A_SUBDIR;
#else
        struct dirent* ent = nullptr;

        DIR* pDir = opendir(folder.c_str());
        while (pDir != nullptr && (ent = readdir(pDir)))
        {
            strName = ent->d_name;
            bDir = ent->d_type & DT_DIR;
#endif
            std::string strFilePath = folder;
            strFilePath += "/";
            strFilePath += strName;

            if (strName == "." || strName == "..")
            {
                int ret = RMDIR(strFilePath.c_str());
                continue;
            }

            if (bDir)
            {
                remove_dir(strFilePath);
            }
            else
            {
                remove_file(strFilePath);
            }

#ifdef _MSC_VER
        } while (_findnext(handle, &info) == 0);

        _findclose(handle);
#else
    }
        if (pDir != nullptr)
            closedir(pDir);
#endif // _MSC_VER
        return true;
}
    bool is_exsit_file(const std::string& filename)
    {
        if (ACCESS(filename.c_str(), 0) == -1)
            return false;

        return true;
    }

    bool is_exsit_folder(const std::string& folder)
    {
        if (ACCESS(folder.c_str(), 0) == -1)
            return false;

        return true;
    }

    std::string get_file_name(const std::string& filePath)
    {
        size_t pos = filePath.find_last_of('/');
        size_t pos2 = filePath.find_last_of('\\');
        size_t index = std::string::npos;

        if (pos != std::string::npos && pos2 != std::string::npos)
        {
            index = max(pos, pos2);
        }
        else
        {
            if (pos == std::string::npos)
                index = pos2;
            else
                index = pos;
        }
        if (index == std::string::npos) return filePath;

        return filePath.substr(index + 1, filePath.size() - index);
    }

    std::string get_file_path(const std::string& filePath)
    {
        std::string strName = get_file_name(filePath);
        std::string strPath = filePath.substr(0, filePath.length() - strName.length());
        return strPath;
    }

    bool load_file(const std::string& filename, std::string& strbuf)
    {
        FILE* psLogFile = nullptr;
        psLogFile = fopen(filename.c_str(), "rb");
        if (psLogFile == nullptr)
            return false;

        size_t len = 0;
        fseek(psLogFile, 0, SEEK_END);
        len = ftell(psLogFile);
        strbuf.resize(len + 1);
        fseek(psLogFile, 0, SEEK_SET);
        fread((char*) strbuf.c_str(), len, 1, psLogFile);
        fclose(psLogFile);
        return true;
    }

    bool write_file(const std::string& filename, const std::string& strbuf)
    {
        FILE* psLogFile = nullptr;
        psLogFile = fopen(filename.c_str(), "wb");
        if (psLogFile == nullptr)
            return false;

        fwrite(strbuf.c_str(), 1, strbuf.size(), psLogFile);
        fclose(psLogFile);
        return true;
    }
}