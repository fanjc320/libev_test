#include "pch.h"
#include "SvrApp.h"
#include "module/ModulePb.h"
#include "module/ModuleShm.h"
#include "module/ModuleTimer.h"
#include "config/ModConfig.h"

bool SvrApp::RegModule()
{
	bool ret = false;
	ret = APP_REG_MODULE(ModConfig::GetInstance(), false, false);
	ERROR_LOG_EXIT0(ret);
	ret = APP_REG_MODULE(ModuleShm::GetInstance(), true, false);
	ERROR_LOG_EXIT0(ret);
	ret = APP_REG_MODULE(ModulePb::GetInstance(), false, false);
	ERROR_LOG_EXIT0(ret);
	ret = APP_REG_MODULE(ModuleTimer::GetInstance(), true, true);
	ERROR_LOG_EXIT0(ret);
	return true;
Exit0:
	return false;
}
