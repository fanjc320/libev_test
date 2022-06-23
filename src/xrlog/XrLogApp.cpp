#include "pch.h"
#include "XrLogApp.h"
#include "module/ModulePb.h"
#include "module/ModuleShm.h"
#include "module/ModuleTimer.h"
#include "session/ModLogSession.h"
#include "config/ModConfig.h"

#include "mysql/ModMysqlAccDB.h"
#include "mysql/ModMysqlLogDB.h"

SvrAppBase* GetMainApp()
{ 
    return XrLogApp::GetInstance();
}

XrLogApp::~XrLogApp()
{

}

int32_t XrLogApp::Init(uint64_t millFrame /*= 20*/)
{
    bool ret = SvrApp::Init(millFrame);
    ERROR_LOG_EXIT0(ret);
    return 1;
Exit0:
    return 0;
}

bool XrLogApp::RegModule()
{
    bool ret = false;
	ret = SvrApp::RegModule();
    ERROR_LOG_EXIT0(ret);
    ret = APP_REG_MODULE(ModLogSession::GetInstance(), true, true);
    ERROR_LOG_EXIT0(ret);
	ret = APP_REG_MODULE(ModMysqlLogDB::GetInstance(), true, false);
	ERROR_LOG_EXIT0(ret);
	ret = APP_REG_MODULE(ModMysqlAccDB::GetInstance(), true, false);
	ERROR_LOG_EXIT0(ret);
    return true;
Exit0:
    return false;
}

void XrLogApp::Stop()
{
    SvrApp::Stop();
}

void XrLogApp::Fini()
{
    SvrApp::Fini();
}

bool XrLogApp::Reload()
{
    bool ret = SvrApp::Reload();
    ERROR_LOG_EXIT0(ret);
    return true;
Exit0:
    return false;
}
