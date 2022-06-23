#pragma once
#include "server/SvrClient.h"
#include "mq/shared/ProgMqBase.h"
#include "stl/map_list.h"
#include "util/HandleEvent.h"

class GatePlayer : public SvrSession
{
public:  
	GatePlayer(int64_t accid);

	virtual void OnDisconnect() override;

public:
	int64_t GetAccId() { return 1; }

protected:
	DECLARE_EVENT1(const MsgHead*);
	bool OnTest(const MsgHead* pMsg);
	virtual bool OnEventDefault(const MsgHead* pMsg) override;

private:
	int64_t _accID = 0;
};