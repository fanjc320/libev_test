#include "pch.h"
#include <typeinfo>

#include "SvrKindHandle.h"
#include "log/LogMgr.h"

bool SvrKindHandle::RegHandle(uint32_t dwModule, uint32_t dwProtoID, FUNC_MSG_HEAD func)
{
    ERROR_LOG_EXIT0(func != nullptr && dwModule < MAX_SVR_KIND_TYPE);
    if (_vecHandle[dwModule].size() <= dwProtoID)
        _vecHandle[dwModule].resize((size_t) dwProtoID + 1, nullptr);

    if (_vecHandle[dwModule][dwProtoID] != nullptr)
    {
        LOG_ERROR("RegHandle exsit %u, %u, %s!", dwModule, dwProtoID, typeid(*this).name());
        return false;
    }

    _vecHandle[dwModule][dwProtoID] = func;
    return true;

Exit0:
    return false;
}

FUNC_MSG_HEAD* SvrKindHandle::GetHandle(uint32_t dwModule, uint32_t dwProtoID)
{
    ERROR_LOG_EXIT0(dwModule < MAX_SVR_KIND_TYPE && _vecHandle[dwModule].size() > dwProtoID);
    return &_vecHandle[dwModule][dwProtoID];
Exit0:
    return nullptr;
}

bool SvrKindHandle::OnHandleMsg(const MsgHead* pMsg)
{
    FUNC_MSG_HEAD* pFunc = GetHandle(pMsg->mod_id, pMsg->proto_id);
    if (pFunc != nullptr)
    {
        return (*pFunc)(pMsg);
    }
    return false;
}

