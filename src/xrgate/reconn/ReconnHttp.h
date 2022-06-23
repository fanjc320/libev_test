#pragma once
#include "dtype.h"
#include "server/mq/SvrProgMq.h"
class ReconnHttp : public SvrProgMq
{
public:
    ReconnHttp();

	DECLARE_EVENT1(const MsgHead*);
	bool OnTest(const MsgHead* pMsg);

};

