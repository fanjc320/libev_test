#include <string.h>
#include <map>
#include "AppConfig.h"
#include "log/LogMgr.h"
#include "string/str_split.h"

#include "server/AppConfigBase.h"

using namespace std;

AppConfig::AppConfig()
{

}

bool AppConfig::Init(AppConfigBase* pConfig)
{
	m_pConfig = pConfig;
	if (pConfig == nullptr) return false;
    return true;
}

sDBSvrConfig AppConfig::GetDB()
{
	sDBSvrConfig cfg;
	return cfg;
}

/************************************************************************/
/* 获取匹配服务器配置信息                                             */
/************************************************************************/
sWorldSvrConfig AppConfig::GetWorld()
{
	sWorldSvrConfig cfg;

    cfg.m_AreaList.resize(3);
	for (uint32_t i = 1; i < 4; ++i)
	{
        cfg.m_AreaList[i] = m_pConfig->ReadInt("xrworld", i, "xrfightcenter", "arean");
	}

	if (!cfg.IsValid())
	{
		LOG_ERROR("Config Error!");
	}
	return cfg;
}

sFCenterSvrConfig AppConfig::GetFightCenter()
{
    sFCenterSvrConfig cfg;
    if (!cfg.IsValid())
    {
        LOG_ERROR("Config Error!");
    }
    return cfg;
}

sFriendSvrConfig AppConfig::GetFriend()
{
	sFriendSvrConfig svrFriendConfig;
	if (!svrFriendConfig.IsValid())
	{
		LOG_ERROR("Config Error!");
	}
	return svrFriendConfig;
}

sLogSvrConfig AppConfig::GetLog()
{
	sLogSvrConfig svrLogConfig;
    svrLogConfig._IsHttpSdk = m_pConfig->ReadAttrBool("xrlog:httpsdk");
	if (!svrLogConfig.IsValid())
	{
		LOG_ERROR("Config Error!");
	}
	return svrLogConfig;
}

sFightSvrConfig AppConfig::GetFight(uint32 id, uint32 maxthread)
{
	sFightSvrConfig cfg;

	cfg._IpPort._ip = m_pConfig->ReadStr("xrfightlist", id, "xrfight", "IP");
	cfg._IpPort._port = m_pConfig->ReadInt("xrfightlist", id, "xrfight", "Port");

    if (!cfg._IpPort.IsValid())
    {
        cfg._IpPort._ip = m_pConfig->ReadStr("xrfightlist", 1, "xrfight", "IP");
        cfg._IpPort._port = m_pConfig->ReadInt("xrfightlist", 1, "xrfight", "Port");

        if (cfg._IpPort.IsValid())
        {
            cfg._IpPort._port += (id - 1)*maxthread;
        }
    }

	if (!cfg.IsValid())
	{
		LOG_ERROR("Config Error! id:%u", id);
	}

	return cfg;
}

sMallSvrConfig AppConfig::GetMall()
{
	sMallSvrConfig cfg;
	cfg._IpPort._ip = m_pConfig->ReadAttrStr("xrmall:IP");
	cfg._IpPort._port = m_pConfig->ReadAttrInt("xrmall:Port");
	if (!cfg.IsValid())
	{
		LOG_ERROR("Config Error!");
	}
	return cfg;
}

/************************************************************************/
/* 游戏服务器配置信息                                                                     */
/************************************************************************/
sGameSvrConfig AppConfig::GetGame()
{
	sGameSvrConfig cfg;
    cfg._gmOpen = m_pConfig->ReadAttrBool("xrgame:gmopen");

	if (!cfg.IsValid())
	{
		LOG_ERROR("Config Error!");
	}

	return cfg;
}

/************************************************************************/
/* 网关服务器配置信息                                                                     */
/************************************************************************/
sGateSvrConfig AppConfig::GetGate(uint32 id)
{
	sGateSvrConfig cfg;
	cfg._IpPort._ip = m_pConfig->ReadAttrStr("xrgate:IP");
	cfg._IpPort._port = m_pConfig->ReadAttrInt("xrgate:Port");

	if (!cfg.IsValid())
	{
		LOG_ERROR("Config Error!");
	}

	cfg._IpPort._port += (id - 1);
	return cfg;
}

sCenterSvrConfig AppConfig::GetCenter()
{
	sCenterSvrConfig cfg;

    if (!cfg.IsValid())
    {
        LOG_ERROR("Config Error!");
    }
	return cfg;
}

sHttpConfig AppConfig::GetHttp()
{
	sHttpConfig cfg;

    if (!cfg.IsValid())
    {
        LOG_ERROR("Config Error!");
    }
	return cfg;
}
