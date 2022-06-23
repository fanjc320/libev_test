
#pragma once

#include <climits>
#include "shm/shm_type.h"
#include <stdint.h>
#include <memory.h>
#include <time.h>
#include "dtype.h"

#ifdef _MSC_VER
#include <windows.h>
#endif // DEBUG

namespace shm_mgr
{
    const size_t   LAST_START_RECORD_COUNT = 5;
    const size_t   POOL_NODE_COUNT = 3;

    struct SHM_FILE_HANDLE
    {
        int32_t fhandle;
        void* handle;
        void* addr;
    };

    struct SHM_STRUCT_POOL
    {
        BOOL dirty_flag;
        int32_t growth_type;        // 增长模式
        int32_t shm_type;           // type
        int32_t cur_node_index;     //现在正在分配的node下标
        int32_t item_data_size;     // 实际置入shm的对象大小
        int32_t item_real_data_size;// 包含头信息和对齐处理后的实际大小
        int32_t item_base_num;      // 初始化的数量
        int64_t item_total_num;     // item数量可用上限
        int64_t item_mapped_num;    // 已经map物理内存的item总数量
        int64_t item_free_num;      // 可分配的item数量
        int64_t pool_real_mem_size;
        SHM_FILE_HANDLE node_array[POOL_NODE_COUNT];
    };

    struct SHM_LAST_USE
    {
        int32_t  from_id;
        uint16_t last_timer;
        uint16_t last_msg;
    };

    struct SHM_MGR
    {
        char            filename[1024];
        BOOL            is_resume;
        time_t          last_resume_sec[LAST_START_RECORD_COUNT]; // 如果是非resume启动，全部为0
        SHM_LAST_USE    last_info;
        SHM_STRUCT_POOL addr_pool[SHM_TYPE_MAX];
    };

    extern bool is_file_handle_valid(SHM_FILE_HANDLE& handle);
    extern void file_handle_init(SHM_FILE_HANDLE& handle);
    extern bool is_file_handle_empty(SHM_FILE_HANDLE& handle);
    extern void pool_init_item_num(SHM_STRUCT_POOL& pool);
}
