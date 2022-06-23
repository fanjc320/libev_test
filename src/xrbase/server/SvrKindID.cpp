#include "pch.h"
#include "SvrKindID.h"

void SvrKindID::SetSvrKind(const std::string& szName, uint32_t dwArean, uint32_t id)
{
    _szSvrName = szName;
    _dwSvrArean = dwArean;
    _dwSvrID = id;
}

void SvrKindID::SetSvrKind(const std::string& szName, uint32_t dwType, uint32_t dwArean, uint32_t id)
{
    _szSvrName = szName;
    _dwSvrArean = dwArean;
    _dwSvrID = id;
    _dwSvrType = dwType;
}

void SvrKindID::SetSvrKind(uint32_t dwArean, uint32_t id)
{
    _dwSvrArean = dwArean;
    _dwSvrID = id;
}

std::string SvrKindID::GetSvrKindKey()
{
    std::string ret = _szSvrName;
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)
    ret = ret + "_d";
#endif
    return ret + "." + std::to_string(_dwSvrArean) + "." + std::to_string(_dwSvrID);
}
