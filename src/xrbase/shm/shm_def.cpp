
#include "pch.h"
#include "shm_def.h"
#include <time.h>

namespace shm_mgr
{
    bool is_file_handle_valid(SHM_FILE_HANDLE& handle)
    {
        return handle.fhandle != -1 && handle.addr != nullptr;
    }

    void file_handle_init(SHM_FILE_HANDLE& handle)
    {
        handle.fhandle = -1;
        handle.addr = nullptr;
        handle.handle = nullptr;
    }

    bool is_file_handle_empty(SHM_FILE_HANDLE& handle)
    {
        return handle.fhandle == -1 && handle.addr == nullptr;
    }

    void pool_init_item_num(SHM_STRUCT_POOL& pool)
    {
        pool.item_data_size = 0;
        pool.item_real_data_size = 0;
        pool.item_base_num = 0;
        pool.item_total_num = 0;
        pool.item_mapped_num = 0;
        pool.item_free_num = 0;
        pool.cur_node_index = -0;
    }
}
