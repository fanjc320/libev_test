#pragma once
#include "dtype.h"

namespace shm_mgr
{
    const uint64_t SHM_POOL_ITEM_MAX = UINT64_MAX; // item������

    enum class EStructGrowType
    {
        EXP = 0, //ָ��
        LINEAR = 1, //����
        DEFAULT = 2, //ƽ�б�������
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
        int64_t  next_item_offset;  // for freelist����������һ���ڵ�Ϊ0
        uint32_t item_serial;      // �����������кţ����Թ����Чmid��Ϊ0��ʾ��ǰitem��δ������
        char     item_data[0];
    };

    struct SHM_STRUCT_HEAD
    {
        int64_t  mapped_item_num;  // �Ѿ�map�����ڴ��item����
        int64_t  free_item_num;    // �ɷ����item����
        int64_t  head_item_offset; // ��ǰpool�е�һ��δ�����item(���������)�����û��Ϊ0
        uint32_t item_serial;
        char     lst_struct[0];
    };
    const uint32_t SIZE_STRUCT_HEAD = sizeof(SHM_STRUCT_HEAD);

}