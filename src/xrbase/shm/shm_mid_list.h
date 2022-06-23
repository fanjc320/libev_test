#pragma once
#include "dtype.h"

// �����������ʵ��˫����(�ǻ���), ��¼�����ڴ��е���������

namespace shm
{
    struct head_mid_node
    {
        uint64_t head_mid = 0; //�������Ϊ�ո�ֵΪ0
        uint64_t tail_mid = 0; //�������Ϊ�ո�ֵΪ0
    };

    struct list_mid_node
    {
        uint64_t prev_mid = 0; //����������׽ڵ��ֵΪ0
        uint64_t next_mid = 0; //���������β�ڵ��ֵΪ0
    };

    int add_head(head_mid_node *head, uint64_t obj_mid);
    int add_tail(head_mid_node *head, uint64_t obj_mid);
    int del_node(head_mid_node *head, uint64_t obj_mid);
}
