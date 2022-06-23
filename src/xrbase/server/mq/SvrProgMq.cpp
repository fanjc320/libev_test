/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "SvrProgMq.h"
#include "net/MsgHead.h"
#include "server/SvrAppBase.h"
#include "string/str_format.h"

SvrProgMq::SvrProgMq(const std::string& szFrom, const std::string& szTo, int32_t max_buf)
    : SvrProgMqBase(szFrom, szTo, max_buf)
{

}

SvrProgMq::SvrProgMq(const std::string& szName, uint32_t dwType)
{
    _szSvrName = szName;
    _dwSvrType = dwType;
}

SvrProgMq::~SvrProgMq()
{

}

bool SvrProgMq::LogicInit()
{
    std::string strApp = SvrAppBase::GetAppKey();
    if (strApp.empty()) return false;
    SetFromTo(strApp, GetSvrKindKey());
    return InitLoad();
}

bool SvrProgMq::IsConnect()
{
    return _tmRecvLast + _check_connect > get_curr_time();
}

void SvrProgMq::OnDisConnect()
{
    LOG_WARNING("name %s not disconnect, from: %s, to: %s", GetSvrKindKey().c_str(), _strFrom.c_str(), _strTo.c_str());
}

void SvrProgMq::OnConnect()
{
    LOG_WARNING("name %s connect, from: %s, to: %s", GetSvrKindKey().c_str(), _strFrom.c_str(), _strTo.c_str());
}

void SvrProgMq::OnFrame(uint64_t qwTm)
{
    if (!IsConnect())
	{
        if (_logConnect.On(qwTm))
        {
			LOG_INFO("name %s not connect, from: %s, to: %s", GetSvrKindKey().c_str(), _strFrom.c_str(), _strTo.c_str());
        }

        if (_isConnect == EConState::Connect || _isConnect == EConState::FirstCon)
        {
            _isConnect = EConState::Discon;
            OnDisConnect();
            return;
        }
    }

	if (!IsInit() || !InitLoad())
		return;

    if (_isConnect == EConState::FirstCon)
	{
		_isConnect = EConState::Connect;
        SendPing();
		OnConnect();
        return;
    }

    if (_tmSendLast + _send_inv < get_curr_time())
    {
        SendPing();
    }
}

void SvrProgMq::OnTick(uint64_t qwTm)
{
	if (!IsInit())
		return;

    MainLoop();
}

bool SvrProgMq::OnHandleMsg(const MsgHead* pMsg)
{
    int ret = OnEvent(MAKE_UINT32(pMsg->mod_id, pMsg->proto_id), pMsg);
    if (ret >= 0) return ret;

    return OnEventDefault(pMsg);
}
