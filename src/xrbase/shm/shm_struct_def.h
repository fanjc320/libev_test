#pragma once
#include "dtype.h"

namespace shm_mgr
{
    const uint64_t SHM_POOL_ITEM_MAX = UINT64_MAX; // item的上限

    enum class EStructGrowType
    {
        EXP = 0, //指数
        LINEAR = 1, //线性
        DEFAULT = 2, //平行倍数增长
    };

    struct SHM_STRUCT_MID
    {
        uint64_t item_serial : 14;
        uint64_t shm_type : 9;
        uint64_t node_index : 4;
        uint64_t item_offset : 37;
    };

    struct SHM_STRUCT
    {
        int64_t  next_item_offset;  // for freelist，如果是最后一个节点为0
        uint32_t item_serial;      // 分配对象的序列号，用以规避无效mid，为0表示当前item尚未被分配
        char     item_data[0];
    };

    struct SHM_STRUCT_HEAD
    {
        int64_t  mapped_item_num;  // 已经map物理内存的item数量
        int64_t  free_item_num;    // 可分配的item数量
        int64_t  head_item_offset; // 当前pool中第一个未分配的item(单链表管理)，如果没有为0
        uint32_t item_serial;
        char     lst_struct[0];
    };
    const uint32_t SIZE_STRUCT_HEAD = sizeof(SHM_STRUCT_HEAD);

}