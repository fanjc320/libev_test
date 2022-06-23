#include "pch.h"
#include "shm_mid_list.h"
#include "shm/shm_mgr.h"
#include "log/LogMgr.h"

namespace shm
{
    int add_head(head_mid_node* head, uint64_t obj_mid)
    {
        list_mid_node* node_new = (list_mid_node*) shm_mgr::mid2ptr(obj_mid);
        assert_retval(node_new, -1);

        list_mid_node* node_1st = (list_mid_node*) shm_mgr::mid2ptr(head->head_mid);
        if (node_1st == nullptr)
        {
            node_new->next_mid = 0;
            node_new->prev_mid = 0;
            head->head_mid = obj_mid;
            head->tail_mid = obj_mid;
        }
        else
        {
            node_new->next_mid = head->head_mid;
            node_new->prev_mid = 0;
            node_1st->prev_mid = obj_mid;
            head->head_mid = obj_mid;
        }

        return 0;
    }

    int add_tail(head_mid_node* head, uint64_t obj_mid)
    {
        list_mid_node* node_new = (list_mid_node*) shm_mgr::mid2ptr(obj_mid);
        assert_retval(node_new, -1);

        list_mid_node* node_last = (list_mid_node*) shm_mgr::mid2ptr(head->tail_mid);
        if (node_last == nullptr)
        {
            node_new->next_mid = 0;
            node_new->prev_mid = 0;
            head->head_mid = obj_mid;
            head->tail_mid = obj_mid;
        }
        else
        {
            node_new->next_mid = 0;
            node_new->prev_mid = head->tail_mid;
            node_last->next_mid = obj_mid;
            head->tail_mid = obj_mid;
        }

        return 0;
    }

    int del_node(head_mid_node* head, uint64_t obj_mid)
    {
        list_mid_node* node = (list_mid_node*) shm_mgr::mid2ptr(obj_mid);
        assert_retval(node, -1);

        list_mid_node* prev = (list_mid_node*) shm_mgr::mid2ptr(node->prev_mid);
        list_mid_node* next = (list_mid_node*) shm_mgr::mid2ptr(node->next_mid);

        if (prev != nullptr)
        {
            prev->next_mid = node->next_mid;
        }

        if (next != nullptr)
        {
            next->prev_mid = node->prev_mid;
        }

        if (head->head_mid == obj_mid)
        {
            head->head_mid = node->next_mid;
        }

        if (head->tail_mid == obj_mid)
        {
            head->tail_mid = node->prev_mid;
        }

        return 0;
    }
}
