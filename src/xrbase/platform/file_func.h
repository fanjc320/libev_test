/*
 *
 *      Author: venture
 */

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <chrono>
#include <vector>
#include <ctype.h>
#include <sys/stat.h>

using namespace std;
#ifdef _MSC_VER
#include <direct.h>
#include <io.h>
#else
#include <stdarg.h>
#include<sys/types.h>
#include<unistd.h>
#include <dirent.h>
#include <sys/sysinfo.h>
#endif

#ifdef _MSC_VER
#define ACCESS(a, b) _access(a, b)
#define MKDIR(a) _mkdir((a))
#define RMDIR(a) _rmdir((a))
#define FILENO(a) _fileno(a)
#else
#define ACCESS(a, b) access(a, b)
#define MKDIR(a) mkdir((a),0777)
#define RMDIR(a)  rmdir(a)
#define FILENO(a) fileno(a)
#endif

namespace file
{
    extern std::string get_app_data_path(const string& strName);
    extern std::string get_full_name();
    extern std::string get_exec_path();
    extern std::string get_local_path(const std::string& szFile);
    extern std::string get_exec_name(bool delDebugFlag = false);

    extern bool get_folder_file(const std::string& szDir, std::vector<std::string>& vecFile
        , bool bRecurse = false, const std::string& szExt = "");

    extern bool get_folder_list(const std::string& szDir, std::vector<std::string>& vecFolder
        , bool bRecurse = false);

    extern bool create_dir(const std::string& src);
    extern bool remove_file(const std::string& filename);
    extern bool remove_dir(const std::string& folder);
    extern bool remove_dir_file(const std::string& folder);

    extern bool is_exsit_file(const std::string& filename);
    extern bool is_exsit_folder(const std::string& folder);

    extern std::string get_file_name(const std::string& filePath);
    extern std::string get_file_path(const std::string& filePath);

    extern bool load_file(const std::string& filename, std::string& strbuf);
    extern bool write_file(const std::string& filename, const std::string& strbuf);

}
