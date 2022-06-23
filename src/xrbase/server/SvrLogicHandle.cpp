/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "SvrLogicHandle.h"

SvrLogicHandle::SvrLogicHandle()
{
}

SvrLogicHandle::~SvrLogicHandle()
{
}

bool SvrLogicHandle::OnHandleLogicMsg(const NetMsg* pNetMsg)
{
    bool ret = LogicHandle::OnHandleLogicMsg(pNetMsg);
    if (!ret)
    {
        return OnHandleMsg(MSG_HEAD_FROM_NET(pNetMsg), pNetMsg->hd);
    }
    return true;
}

bool SvrLogicHandle::OnHandleMsg(const MsgHead* pMsg, const HostHd& hd)
{
    return true;
}
