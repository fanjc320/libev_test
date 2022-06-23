#pragma once
#include "dtype.h"

// 这个数据用于实现双链表(非环形), 记录共享内存中的数据链表

namespace shm
{
    struct head_mid_node
    {
        uint64_t head_mid = 0; //如果链表为空该值为0
        uint64_t tail_mid = 0; //如果链表为空该值为0
    };

    struct list_mid_node
    {
        uint64_t prev_mid = 0; //如果是链表首节点该值为0
        uint64_t next_mid = 0; //如果是链表尾节点该值为0
    };

    int add_head(head_mid_node *head, uint64_t obj_mid);
    int add_tail(head_mid_node *head, uint64_t obj_mid);
    int del_node(head_mid_node *head, uint64_t obj_mid);
}
