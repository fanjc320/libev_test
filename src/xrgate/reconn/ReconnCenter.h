#pragma once
#include "dtype.h"
#include "server/mq/SvrProgMq.h"

class ReconnCenter : public SvrProgMq
{
public:
    ReconnCenter();

    DECLARE_EVENT1(const MsgHead*);
    bool OnTest(const MsgHead* pMsg);
};