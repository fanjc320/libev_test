/*
 *
 *      Author: venture
 */
#include <memory>
#ifndef JEMALLOC_NO_DEMANGLE
    #define JEMALLOC_NO_DEMANGLE
#endif
#include "jemalloc/jemalloc.h"
#include "malloc_x.h"

void* msg_alloc(size_t nSize)
{
	return je_malloc(nSize);
}

void* msg_relloc(void* pMsg, size_t nSize)
{
	return je_realloc(pMsg, nSize);
}

void* msg_calloc(size_t num, size_t size)
{
	return je_calloc(num, size);
}

void msg_free(void* pMsg)
{
	je_free(pMsg);
}

BufItem* buf_item_alloc(uint32_t nsize)
{
    uint8_t* p = (uint8_t*) msg_alloc(nsize);
    if (p == nullptr) return nullptr;

    BufItem* pMsg = new (p)BufItem();
    return pMsg;
}
