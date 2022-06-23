/*
 *
 *      Author: venture
 */

#include "pch.h"
#include "SvrKindList.h"
#include "log/LogMgr.h"

SvrKindID** SvrKindList::GetID(uint32_t dwSvrKind, uint32_t dwSvrID)
{
    auto itr = _vecSvr.find(dwSvrKind);
    if (itr != _vecSvr.end()) itr->second.find(dwSvrID);
    return nullptr;
}

bool SvrKindList::AddID(uint32_t dwSvrKind, uint32_t dwSvrID, SvrKindID* pAdd)
{
    if (GetID(dwSvrKind, dwSvrID) != nullptr)
    {
        LOG_ERROR("AddID exsit %u, %u, %s!", dwSvrKind, dwSvrID, pAdd->GetSvrName().c_str());
        return false;
    }

    _vecSvr[dwSvrKind].insert(dwSvrID, pAdd);
    return true;
Exit0:
    return false;
}

void SvrKindList::DelID(uint32_t dwSvrKind, uint32_t dwSvrID)
{
    auto itr = _vecSvr.find(dwSvrKind);
    if (itr != _vecSvr.end())
        itr->second.erase(dwSvrID);
}

bool SvrKindList::HasID(uint32_t dwSvrKind, uint32_t dwSvrID)
{
    return GetID(dwSvrKind, dwSvrID) != nullptr;
}

size_t SvrKindList::Size(uint32_t dwSvrKind)
{
    auto itr = _vecSvr.find(dwSvrKind);
    if (itr != _vecSvr.end()) return itr->second.size();
    return 0;
}

map_list<uint32_t, SvrKindID*>* SvrKindList::GetKindVec(uint32_t dwSvrKind)
{
    auto itr = _vecSvr.find(dwSvrKind);
    if (itr != _vecSvr.end()) return &itr->second;
    return nullptr;
}

void SvrKindList::Clear()
{
    _vecSvr.clear();
}
