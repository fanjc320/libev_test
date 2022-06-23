#pragma once
#include "dtype.h"
#include "SvrKindType.h"
#include <vector>
#include "net/MsgParser.h"

class SvrKindHandle
{
public:
    bool RegHandle(uint32_t dwModule, uint32_t dwProto, FUNC_MSG_HEAD func);
    FUNC_MSG_HEAD* GetHandle(uint32_t dwModule, uint32_t dwProto);

    virtual bool OnHandleMsg(const MsgHead* pMsg);
protected:
    typedef std::vector<FUNC_MSG_HEAD> VecFuncHandle;
    VecFuncHandle _vecHandle[MAX_SVR_KIND_TYPE];
};