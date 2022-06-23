#pragma once
#include "dtype.h"
#include "server/mq/SvrProgMq.h"
class ReconnMail : public SvrProgMq
{
public:
    ReconnMail();

	DECLARE_EVENT1(const MsgHead*);
	bool OnTest(const MsgHead* pMsg);

};