#include "pch.h"
#include "GateHandle.h"
#include "math/math_basic.h"
#include "log/LogMgr.h"
#include "net/LogicThread.h"
#include "config/ModConfig.h"
#include "server/SvrAppBase.h"
#include "client/MsgModule.pb.h"
#include "client/MsgLogin.pb.h"
#include "pb/pb_parse.h"
#include "session/GatePlayer.h"
#include "session/GatePlayerMgr.h"
#include "time/time_func_x.h"
#include "PbConst.h"
#include "reconn/ModGateReconn.h"

bool GateHandle::LogicInit(LogicThread* pLogicThread)
{
	NetSetting setting;
	sGateSvrConfig cfg = ModConfig::GetInstance()->GetApp().GetGate(GetMainApp()->GetIndex());
	if (!cfg.IsValid()) return false;
	setting.port = cfg._IpPort._port;

	setting.Init(1, 512, MEM_SIZE_64K, MEM_SIZE_1M, MEM_SIZE_1M);
	ERROR_LOG_EXIT0(pLogicThread->InitNetSetting(setting));
	pLogicThread->RegInterfaceListen(this);

	return true;
Exit0:
	return false;
}

void GateHandle::OnFrame(uint64_t qwTick)
{
}

void GateHandle::OnTick(uint64_t qwTick)
{

}

bool GateHandle::OnReload()
{
	return true;
}

bool GateHandle::OnStop()
{
	return true;
}

void GateHandle::Fini()
{

}

bool GateHandle::OnHandleMsg(const MsgHead* pMsg, const HostHd& hd)
{
	switch (pMsg->mod_id)
	{
		//这里只有login会进来
	case MsgMod::Login:
		OnModLogin(pMsg, hd);
		break;
	default:
		return true;
	}
	return true;
Exit0:
	return false;
}

bool GateHandle::OnModLogin(const MsgHead* pMsg, const HostHd& hd)
{
	switch (pMsg->proto_id)
	{
	case MsgMod_Login::Msg_Login_VerifyAccount_CS:
	{
		Msg_Login_VerifyAccount_CS msg;
		PARSE_PTL_HEAD_RET(msg, pMsg);

		LOG_INFO("user: %s, pwd: %s", msg.szusername().c_str(), msg.szpassword().c_str());
		GatePlayer* pPlayer = GatePlayerMgr::GetInstance()->Newplayer(msg.uiaccid());
		Msg_Login_VerifyAccount_SC sc;
		uint32_t code = Code_Common_Succeed;
		sc.set_uiaccid(msg.uiaccid());
		sc.set_estate(Offline);
		sc.set_szusername(msg.szusername());
		sc.set_nservertime(get_curr_time());
		sc.set_opendate(get_curr_time());
		sc.set_ntimezone(get_time_zone());
		sc.set_ngameid(1);

		if (pPlayer == nullptr)
			code = Code_Common_Failure;
		else
		{
			AddSession(pPlayer, hd, 0);
			ModGateReconn::GetInstance()->TransClientToSvr(SVR_KIND_GAME, pPlayer, pMsg);
		}
		pPlayer->SendMsg(&sc, MsgMod_Login::Msg_Login_VerifyAccount_SC, MsgMod::Login, code, pMsg->seq_id);
	}
	break;
		break;
	default:
		break;
	}
	LOG_INFO("recv login");
	return true;
}
