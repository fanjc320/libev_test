/*
 *
 *      Author: venture
 */
#pragma once
#include <stdint.h>
#include "dtype.h"

extern void* msg_alloc(size_t nSize);
extern void* msg_relloc(void* pMsg, size_t nSize);
extern void* msg_calloc(size_t num, size_t size);
extern void  msg_free(void* pMsg);

#define MSG_FREE(pMsg) { msg_free(pMsg); pMsg = nullptr; }

#pragma pack(push)
#pragma pack(1)

struct BufItem
{
    uint32_t size = 0;
    uint8_t  body[0];
};

const uint32_t BUF_ITEM_SIZE = sizeof(BufItem);

#pragma pack( pop )
BufItem* buf_item_alloc(uint32_t nsize);