#pragma once
#include "server/mq/SvrProgMq.h"
#include "dtype.h"

class GateSession :
    public SvrProgMq
{
public:
	GateSession();

	DECLARE_EVENT1(const MsgHead*);
	bool OnTest(const MsgHead* pMsg);
};

