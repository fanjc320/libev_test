#pragma once
#include "dtype.h"
#include "server/mq/SvrProgMq.h"
class ReconnGame : public SvrProgMq
{ 
public:
    ReconnGame();

	DECLARE_EVENT1(const MsgHead*);
	bool OnTest(const MsgHead* pMsg);

};

