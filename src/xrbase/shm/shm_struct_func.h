#pragma once
#include "dtype.h"

namespace shm_mgr
{
    // ����item����������, δ���þ���SHM_STRUCT_POOL(������)
    extern int64_t  get_max_num(int32_t shm_type);
    extern int32_t  set_max_num(int32_t shm_type, int64_t max_num);
    extern int64_t  get_max_free_num(int32_t shm_type);
    extern int64_t  get_mapped_num(int32_t shm_type);// ��ǰ�Ѿ����������item������
    extern int64_t  get_total_num(int32_t shm_type);// ��ǰ����item����������
    extern int64_t  get_free_num(int32_t shm_type);// ��ǰ����item����
    extern int64_t  get_used_num(int32_t shm_type);// ��ǰ��ʹ��item����

    extern int32_t  get_item_mem_size(int32_t shm_type);
    extern int32_t  get_item_real_mem_size(int32_t shm_type);
    extern uint64_t get_struct_mem_size(int32_t shm_type);
    extern uint64_t get_struct_used_mem_size(int32_t shm_type);
    extern uint64_t get_struct_mem_size();
    extern uint64_t get_struct_used_mem_size();
};