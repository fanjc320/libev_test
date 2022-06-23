/*
 *
 *      Author: venture
 */
#pragma once
#include "dtype.h"
#include "net/MsgHead.h"
#include "stl/map_list.h"
#include "server/mq/SvrProgMq.h"
#include <unordered_map>

class SvrProgMqHandle
{
public:
    SvrProgMqHandle();
    ~SvrProgMqHandle();

    template< typename T>
    T* GetReconT(uint32_t key);

    template<typename T>
    T* AddReconT();
    void FreeReconT();

    SvrProgMq* GetRecon(uint32_t key);
    bool  AddRecon(SvrProgMq* p);
    
    map_list<uint32_t, SvrProgMq*>& GetMap() { return _mapMq; }

    void Clear();

protected:
    map_list<uint32_t, SvrProgMq*> _mapMq;
    uint32_t key = 0;
};

template< typename T>
T* SvrProgMqHandle::GetReconT(uint32_t key)
{
    SvrProgMq* p = GetRecon(key);
    if (p == nullptr) return NULL;
    return dynamic_cast<T*>(p);
}

template<typename T>
T* SvrProgMqHandle::AddReconT()
{
    T* p = new T();
    if (AddRecon(p)) return p;
    delete p;
    return nullptr;
}


#define AddConnT(arean, id, P, T) \
    P = _Handle.AddReconT<T>(); \
    ERROR_LOG_EXIT0(P != nullptr);\
    P->SetSvrKind((arean), (id)); \
    ERROR_LOG_EXIT0(P->LogicInit());

