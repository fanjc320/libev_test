#pragma once
#include "dtype.h"

namespace shm_mgr
{
    // 可用item数量的上限, 未配置就是SHM_STRUCT_POOL(无上限)
    extern int64_t  get_max_num(int32_t shm_type);
    extern int32_t  set_max_num(int32_t shm_type, int64_t max_num);
    extern int64_t  get_max_free_num(int32_t shm_type);
    extern int64_t  get_mapped_num(int32_t shm_type);// 当前已经物理申请的item总数量
    extern int64_t  get_total_num(int32_t shm_type);// 当前可用item数量的上限
    extern int64_t  get_free_num(int32_t shm_type);// 当前空闲item数量
    extern int64_t  get_used_num(int32_t shm_type);// 当前已使用item数量

    extern int32_t  get_item_mem_size(int32_t shm_type);
    extern int32_t  get_item_real_mem_size(int32_t shm_type);
    extern uint64_t get_struct_mem_size(int32_t shm_type);
    extern uint64_t get_struct_used_mem_size(int32_t shm_type);
    extern uint64_t get_struct_mem_size();
    extern uint64_t get_struct_used_mem_size();
};