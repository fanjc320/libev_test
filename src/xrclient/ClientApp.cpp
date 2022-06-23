#include "pch.h"
#include "ClientApp.h"
#include "module/ModulePb.h"
#include "module/ModuleShm.h"
#include "module/ModuleTimer.h"
#include "ClientModuleReconn.h"
#include "ClientArgOpt.h"

SvrAppBase* GetMainApp()
{ 
    return ClientApp::GetInstance();
}

ClientApp::~ClientApp()
{

}

int32_t ClientApp::Init(uint64_t millFrame /*= 20*/)
{
    bool ret = SvrApp::Init(millFrame);
    ERROR_LOG_EXIT0(ret);
    return 1;
Exit0:
    return 0;
}

bool ClientApp::Arg(int argc, char* argv[])
{
	SetAutoConfig(false);
	SetAutoOpt(false);
	SetArg<ClientArgOpt>();
	bool ret = SvrAppBase::Arg(argc, argv);
	ERROR_LOG_EXIT0(ret);
	return true;

Exit0:
	return false;
}

ClientArgOpt* ClientApp::GetClientOpt()
{
	return GetArg<ClientArgOpt>();
}

bool ClientApp::RegModule()
{
	bool ret = false;
	ret = APP_REG_MODULE(ModuleShm::GetInstance(), true, false);
	ERROR_LOG_EXIT0(ret);
	ret = APP_REG_MODULE(ModulePb::GetInstance(), false, false);
	ERROR_LOG_EXIT0(ret);
	ret = APP_REG_MODULE(ModuleTimer::GetInstance(), true, true);
	ERROR_LOG_EXIT0(ret);
	ret = APP_REG_MODULE(ClientModuleReconn::GetInstance(), true, false);
	ERROR_LOG_EXIT0(ret);

	return true;
Exit0:
	return false;
}

void ClientApp::Stop()
{
    SvrApp::Stop();
}

void ClientApp::Fini()
{
    SvrApp::Fini();
}

bool ClientApp::Reload()
{
    bool ret = SvrApp::Reload();
    ERROR_LOG_EXIT0(ret);
    return true;
Exit0:
    return false;
}
