#ifndef _GAMECONFIG_H_
#define _GAMECONFIG_H_
#include <string.h>
#include <map>
#include "dtype.h"
#include "config/ConfigStruct.h"

class AppConfigBase;

class AppConfig
{
public:
    AppConfig();
    bool   Init(AppConfigBase* pConfig = nullptr);

	//数据库
	sDBSvrConfig GetDB();

	sWorldSvrConfig GetWorld();
    sFCenterSvrConfig GetFightCenter();

	sFriendSvrConfig GetFriend();
	sLogSvrConfig GetLog();
	sFightSvrConfig GetFight(uint32 id, uint32 maxthread);
	sMallSvrConfig GetMall();
	sGameSvrConfig GetGame();
	sGateSvrConfig GetGate(uint32 id);
    sCenterSvrConfig GetCenter();
	sHttpConfig GetHttp();

private:
	AppConfigBase*  m_pConfig = nullptr;
};

#endif