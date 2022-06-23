#pragma once
#include "dtype.h"
#include "server/mq/SvrProgMq.h"
class ReconnFriend : public SvrProgMq
{
public:
    ReconnFriend();

	DECLARE_EVENT1(const MsgHead*);
	bool OnTest(const MsgHead* pMsg);

};