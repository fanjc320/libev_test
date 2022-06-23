#pragma once
#include <stdint.h>
#include "log/LogMgr.h"

struct linked_list_node
{
    linked_list_node* pPrev = nullptr;
    linked_list_node* pNext = nullptr;
};

inline void clear_list_node(linked_list_node* pNode)
{
    pNode->pPrev = nullptr;
    pNode->pNext = nullptr;
}

struct linked_list
{
public:
    linked_list() {}
    ~linked_list() {}

    inline void init(void);

    inline void uninit(void);

    inline bool empty(void);

    inline linked_list_node& head(void);
    inline linked_list_node& tail(void);

    static inline bool insert_after(linked_list_node* pNewNode, linked_list_node* pInsertAfter);
    static inline bool insert_before(linked_list_node* pNewNode, linked_list_node* pInsertAfter);

    static inline bool remove(linked_list_node* pNode);

    inline bool push_head(linked_list_node* pNode);
    inline linked_list_node* pop_head(void);

    inline bool push_tail(linked_list_node* pNode);
    inline linked_list_node* pop_tail(void);

    inline void merge_from(linked_list& OtherList);

private:
    linked_list_node  _head;
    linked_list_node  _tail;
};

inline void linked_list::init(void)
{
    _head.pPrev = nullptr;
    _head.pNext = &_tail;
    _tail.pPrev = &_head;
    _tail.pNext = nullptr;
}

inline void linked_list::uninit(void)
{
    CHECK_LOG_ERROR(_head.pPrev == nullptr);
    CHECK_LOG_ERROR(_head.pNext == &_tail);
    CHECK_LOG_ERROR(_tail.pPrev == &_head);
    CHECK_LOG_ERROR(_tail.pNext == nullptr);
}

inline bool linked_list::empty()
{
#ifdef _DEBUG
    ERROR_LOG_EXIT0(_head.pPrev == nullptr);
    ERROR_LOG_EXIT0(_tail.pNext == nullptr);
    ERROR_EXIT0(_head.pNext == &_tail);
#endif	// _DEBUG

    ERROR_EXIT0(_tail.pPrev == &_head);

    return true;
Exit0:
    return false;
}

inline linked_list_node& linked_list::head(void)
{
    return _head;
}

inline linked_list_node& linked_list::tail(void)
{
    return _tail;
}

inline bool linked_list::insert_after(linked_list_node* pNewNode, linked_list_node* pTargetNode)
{
    ERROR_LOG_EXIT0(pNewNode && pTargetNode);
    ERROR_LOG_EXIT0(pNewNode->pPrev == nullptr && pNewNode->pNext == nullptr);
    ERROR_LOG_EXIT0(pTargetNode->pNext);

    pNewNode->pPrev = pTargetNode;
    pNewNode->pNext = pTargetNode->pNext;
    pTargetNode->pNext->pPrev = pNewNode;
    pTargetNode->pNext = pNewNode;

    return true;
Exit0:

    return false;
}

inline bool linked_list::insert_before(linked_list_node* pNewNode, linked_list_node* pTargetNode)
{
    ERROR_LOG_EXIT0(pNewNode && pTargetNode);
    ERROR_LOG_EXIT0(pNewNode->pPrev == nullptr && pNewNode->pNext == nullptr);
    ERROR_LOG_EXIT0(pTargetNode->pPrev);

    pNewNode->pNext = pTargetNode;
    pNewNode->pPrev = pTargetNode->pPrev;
    pTargetNode->pPrev->pNext = pNewNode;
    pTargetNode->pPrev = pNewNode;

    return true;
Exit0:
    return false;
}

inline bool linked_list::remove(linked_list_node* pNode)
{
    ERROR_LOG_EXIT0(pNode);
    ERROR_EXIT0(pNode->pPrev && pNode->pNext);

    pNode->pPrev->pNext = pNode->pNext;
    pNode->pNext->pPrev = pNode->pPrev;

    pNode->pNext = pNode->pPrev = nullptr;

    return true;
Exit0:

    return false;
}

inline bool linked_list::push_head(linked_list_node* pNode)
{
    ERROR_LOG_EXIT0(pNode->pNext == nullptr && pNode->pPrev == nullptr);

    pNode->pPrev = &_head;
    pNode->pNext = _head.pNext;

    pNode->pNext->pPrev = pNode;
    _head.pNext = pNode;

    return true;
Exit0:
    return false;
}

inline linked_list_node* linked_list::pop_head(void)
{
    linked_list_node* pResult = nullptr;

    ERROR_EXIT0(_head.pNext != &_tail);

    pResult = _head.pNext;
    _head.pNext = pResult->pNext;
    pResult->pNext->pPrev = &_head;

    pResult->pPrev = pResult->pNext = nullptr;

    return pResult;
Exit0:
    return nullptr;
}

inline bool linked_list::push_tail(linked_list_node* pNode)
{
    ERROR_LOG_EXIT0(pNode->pNext == nullptr && pNode->pPrev == nullptr);

    pNode->pPrev = _tail.pPrev;
    pNode->pNext = &_tail;

    _tail.pPrev = pNode;
    pNode->pPrev->pNext = pNode;

    return true;
Exit0:
    return false;
}

inline linked_list_node* linked_list::pop_tail(void)
{
    linked_list_node* pResult = nullptr;

    ERROR_EXIT0(_tail.pPrev != &_head);

    pResult = _tail.pPrev;
    _tail.pPrev = pResult->pPrev;
    pResult->pPrev->pNext = &_tail;

    pResult->pPrev = pResult->pNext = nullptr;

    return pResult;
Exit0:
    return nullptr;
}

inline void linked_list::merge_from(linked_list& OtherList)
{
    int32_t nRetCode = 0;
    linked_list_node* pTailNode1 = nullptr;
    linked_list_node* pTailNode2 = nullptr;
    linked_list_node* pOtherHeadNode = nullptr;

    ERROR_EXIT0(!OtherList.empty());

    pTailNode1 = _tail.pPrev;
    pTailNode2 = OtherList._tail.pPrev;

    pOtherHeadNode = OtherList.head().pNext;

    nRetCode = insert_after(pOtherHeadNode, pTailNode1);
    ERROR_LOG_EXIT0(nRetCode);

    OtherList.init();

    return;
Exit0:
    return;
}
