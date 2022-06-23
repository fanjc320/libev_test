/*
 *
 *      Author: venture
 */
#pragma once
#include "dtype.h"
#include <stdint.h>
#include "stl/map_list.h"
#include "server/SvrKindID.h"
#include "server/SvrKindType.h"

class SvrKindList
{
public:
    SvrKindID** GetID(uint32_t dwSvrKind, uint32_t dwSvrID);

    template <class T>
    T* GetServer(uint32_t dwSvrKind, uint32_t dwSvrID)
    {
        SvrKindID** p = GetID(dwSvrKind, dwSvrID);
        if (p == nullptr) return nullptr;
        return dynamic_cast<T*>(*p);
    }

    bool AddID(uint32_t dwSvrKind, uint32_t dwSvrID, SvrKindID* pAdd);
    void DelID(uint32_t dwSvrKind, uint32_t dwSvrID);
    bool HasID(uint32_t dwSvrKind, uint32_t dwSvrID);

    size_t Size(uint32_t dwSvrKind);
    map_list<uint32_t, SvrKindID*>* GetKindVec(uint32_t dwSvrKind);

    void Clear();
private:
    typedef map_list<uint32_t, SvrKindID*>      MapSvrKindID;
    umap<uint32_t, MapSvrKindID>  _vecSvr;
};