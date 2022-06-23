#include "pch.h"
#include "ClientReconn.h"
#include "string/str_format.h"
#include "client/MsgLogin.pb.h"
#include "client/MsgModule.pb.h"
#include "PbConst.h"
#include "ClientApp.h"
#include "ClientArgOpt.h"
#include "pb/pb_parse.h"

BEGIN_EVENT1(ClientReconn, const MsgHead*)
EVENT(MsgMod::Login, MsgMod_Login::Msg_Login_VerifyAccount_SC,  &ClientReconn::OnLogin_VerifyAccount_SC)
END_EVENT1()

map_list<int64_t, ClientReconn*> g_mapReconn;
void ClientReconn::OnConnect()
{
    LOG_WARNING("Connect to %s:%d success", GetIpStr(), GetPort());
    SvrReconn::OnConnect();
    Msg_Login_VerifyAccount_CS msg;
    msg.set_uiaccid(100);
    std::string user = ClientApp::GetClientOpt()->GetUserName();
    std::string pwd = ClientApp::GetClientOpt()->GetUserPwd();
    msg.set_szusername(user);
    msg.set_szpassword(pwd);
    msg.set_dwsdk(1);
    msg.set_un8relogin(2);
    msg.mutable_gameversion()->set_appver("111");
    msg.mutable_gameversion()->set_resver("222");
    std::string ret = msg.SerializeAsString();

    SendMsg(&msg, MsgMod_Login::Msg_Login_VerifyAccount_CS, MsgMod::Login, 0, 0);
}

void ClientReconn::OnDisconnect()
{
    LOG_ERROR("Link Disconnect %s.%d.%d !!!", GetSvrName().c_str(), GetSvrArean(), GetSvrID());
    SvrReconn::OnDisconnect();
}

bool ClientReconn::OnEventDefault(const MsgHead* pMsg)
{
    return true;
}

bool ClientReconn::OnLogin_VerifyAccount_SC(const MsgHead* pMsg)
{
    Msg_Login_VerifyAccount_SC msg;
    PARSE_PTL_HEAD_RET(msg, pMsg);

    LOG_INFO("user: %s, code: %d", msg.szusername().c_str(), pMsg->code);
    return true;
}

ClientReconn* ClientReconn::New()
{
    return new ClientReconn();
}

void ClientReconn::OnFrame()
{

}

void ClientReconn::OnTick()
{

}
