/*
 *
 *      Author: venture
 */
#pragma once
#include "net/NetInclude.h"
#include "net/LogicHandle.h"

class SvrLogicHandle : public LogicHandle
{
public:
    SvrLogicHandle();
    ~SvrLogicHandle();

    virtual bool OnHandleLogicMsg(const NetMsg* pNetMsg) override;
    virtual bool OnHandleMsg(const MsgHead* pMsg, const HostHd& hd);
};

