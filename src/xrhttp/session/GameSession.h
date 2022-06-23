#pragma once
#include "server/mq/SvrProgMq.h"
#include "dtype.h"

class GameSession :
    public SvrProgMq
{
public:
	GameSession();

	DECLARE_EVENT1(const MsgHead*);
	bool OnTest(const MsgHead* pMsg);
};

