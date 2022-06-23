#pragma once
#include "dtype.h"
#include "server/SvrClient.h"
#include "mq/shared/ProgMqBase.h"
#include "util/HandleEvent.h"
#include "stl/map_list.h"

class ClientReconn : public SvrReconn
{
public:
    static ClientReconn* New();
    void OnFrame();
    void OnTick();

private:
    virtual void OnConnect() override;
    virtual void OnDisconnect() override;
	virtual bool OnEventDefault(const MsgHead* pMsg) override;

	DECLARE_EVENT1(const MsgHead*);

    bool OnLogin_VerifyAccount_SC(const MsgHead* pMsg);

};

extern map_list<int64_t, ClientReconn*> g_mapReconn;