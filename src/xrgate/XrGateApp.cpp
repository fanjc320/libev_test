#include "pch.h"
#include "XrGateApp.h"
#include "data/ModGameData.h"
#include "module/ModulePb.h"
#include "module/ModuleShm.h"
#include "module/ModuleTimer.h"
#include "reconn/ModGateReconn.h"
#include "config/ModConfig.h"
#include "session/ModGateSession.h"

SvrAppBase* GetMainApp()
{
    return XrGateApp::GetInstance();
}

XrGateApp::~XrGateApp()
{

}

int32_t XrGateApp::Init(uint64_t millFrame /*= 20*/)
{
    bool ret = SvrApp::Init(millFrame);
    ERROR_LOG_EXIT0(ret);
    return 1;
Exit0:
    return 0;
}

bool XrGateApp::RegModule()
{
    bool ret = false;
	ret = SvrApp::RegModule();
	ERROR_LOG_EXIT0(ret);
    ret = APP_REG_MODULE(ModGameData::GetInstance(), false, false);
    ERROR_LOG_EXIT0(ret);
	ret = APP_REG_MODULE(ModGateReconn::GetInstance(), true, true);
	ERROR_LOG_EXIT0(ret);
	ret = APP_REG_MODULE(ModGateSession::GetInstance(), true, true);
	ERROR_LOG_EXIT0(ret);
    return true;
Exit0:
    return false;
}

void XrGateApp::Stop()
{
    SvrApp::Stop();
}

void XrGateApp::Fini()
{
    SvrApp::Fini();
}

bool XrGateApp::Reload()
{
    bool ret = SvrApp::Reload();
    ERROR_LOG_EXIT0(ret);
    return true;
Exit0:
    return false;
}
