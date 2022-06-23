#include "pch.h"
#include "XrHttpApp.h"
#include "module/ModulePb.h"
#include "module/ModuleShm.h"
#include "module/ModuleTimer.h"
#include "session/ModHttpSession.h"
#include "config/ModConfig.h"
#include "redis/ModConfigRedis.h"

SvrAppBase* GetMainApp()
{
    return XrHttpApp::GetInstance();
}

XrHttpApp::~XrHttpApp()
{

}

int32_t XrHttpApp::Init(uint64_t millFrame /*= 20*/)
{
    bool ret = SvrApp::Init(millFrame);
    ERROR_LOG_EXIT0(ret);
    return 1;
Exit0:
    return 0;
}

bool XrHttpApp::RegModule()
{
    bool ret = false;
	ret = SvrApp::RegModule();
	ERROR_LOG_EXIT0(ret);
	ret = APP_REG_MODULE(ModHttpSession::GetInstance(), true, true);
	ERROR_LOG_EXIT0(ret);
	ret = APP_REG_MODULE(ModConfigRedis::GetInstance(), true, false);
	ERROR_LOG_EXIT0(ret);
    return true;
Exit0:
    return false;
}

void XrHttpApp::Stop()
{
    SvrApp::Stop();
}

void XrHttpApp::Fini()
{
    SvrApp::Fini();
}

bool XrHttpApp::Reload()
{
    bool ret = SvrApp::Reload();
    ERROR_LOG_EXIT0(ret);
    return true;
Exit0:
    return false;
}
