#include "pch.h"
#include "GatePlayer.h"
#include "string/str_format.h"
#include "net/MsgParser.h"
#include "GatePlayerMgr.h"
#include "client/MsgModule.pb.h"
#include "reconn/ModGateReconn.h"
#include "PbConst.h"

BEGIN_EVENT1(GatePlayer, const MsgHead*)
EVENT(1, 1, &GatePlayer::OnTest)
END_EVENT1()


GatePlayer::GatePlayer(int64_t accid)
{
    _accID = accid;
}

void GatePlayer::OnDisconnect()
{
    SvrSession::OnDisconnect();
    GatePlayerMgr::GetInstance()->AddDeletePlayer(this);
}

bool GatePlayer::OnTest(const MsgHead* pMsg)
{
    return true;
}

bool GatePlayer::OnEventDefault(const MsgHead* pMsg)
{
    switch (pMsg->mod_id)
    {
    case MsgMod::Friend:
        return ModGateReconn::GetInstance()->TransClientToSvr(SVR_KIND_FRIEND, this, pMsg);
		break;
    default:
        return ModGateReconn::GetInstance()->TransClientToSvr(SVR_KIND_GAME, this, pMsg);
        break;
    }
    return true;
}