#pragma once
#include "dtype.h"
#include "server/mq/SvrProgMq.h"

class ReconnLog : public SvrProgMq
{ 
public:
    ReconnLog();

	DECLARE_EVENT1(const MsgHead*);
	bool OnTest(const MsgHead* pMsg);

};

