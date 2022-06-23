/*
 *
 *      Author: venture
 */
#pragma once
#include <stdint.h>
#include <string>

namespace shm
{
    typedef void* HANDLE_T;

    extern std::string shm_mk_key(const std::string& szFrom, const std::string& szTo);
    extern std::string shm_file_path();
    extern std::string shm_file_path(const std::string& szShmName);

    extern HANDLE_T shm_create_x(const std::string& szShmName, int64_t nSize, void** pAddr, int32_t& nHandle);
    extern HANDLE_T shm_open_x(const std::string& szShmName, void** pAddr, int64_t& size, int32_t& nHandle);
    extern HANDLE_T shm_open_readonly(const std::string& szShmName, void** pAddr, int64_t& nSize, int32_t& nHandle);
    extern bool shm_find_x(const std::string& szShmName);

    extern bool shm_close_handle(HANDLE_T handle, int32_t& nHandle);
    extern bool shm_munmap(void* pAddr, int64_t nSize);
    extern bool shm_unlink_x(const std::string& name);
    extern bool shm_destroy(const std::string& szShmName);
    extern bool shm_delete_file(const std::string& szShmName);
}
