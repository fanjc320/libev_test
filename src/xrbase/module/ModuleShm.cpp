#include "pch.h"
#include "ModuleShm.h"
#include "shm/shm_mgr.h"
#include "log/LogMgr.h"
#include "server/SvrAppBase.h"
#include "shm/shm_conf.h"
#include "platform/file_func.h"
#include "time/time_func_x.h"
#include "const.h"
ModuleShm::ModuleShm() : SvrModuleBase("ModuleShm")
{

}

bool ModuleShm::DBInit()
{
    int ret = false;
    std::string strShm;
    std::string ini_name;

    strShm = DEF_APP_SHM_HEAD + SvrAppBase::GetAppKey();
    strShm = str_to_lower(strShm);

    ini_name = file::get_local_path(DEF_INI_PATH + SvrAppBase::GetAppName() + ".ini");

#ifdef _DEBUG
    shm_mgr::shm_conf_init(ini_name.c_str(), true);
#else
    shm_mgr::shm_conf_init(ini_name.c_str(), false);
#endif // DEBUG

    shm_mgr::init_type_name();
    ret = shm_mgr::init_shm(strShm.c_str(), IsResume(), true);
    ERROR_LOG_EXIT0(ret == 0);
    SetResume(shm_mgr::is_resume());
    return true;
Exit0:
    return false;
}

bool ModuleShm::LogicInit()
{
    return SvrModuleBase::LogicInit();
}

void ModuleShm::LogicFini()
{
    int ret = shm_mgr::fini_shm();
    CHECK_LOG_RETNONE(ret == 0);
}

bool ModuleShm::Reload()
{
    return SvrModuleBase::Reload();
}

bool ModuleShm::ReloadNotice()
{
    return SvrModuleBase::ReloadNotice();
}

bool ModuleShm::PreReload()
{
    return SvrModuleBase::PreReload();
}

void ModuleShm::Stop()
{
    //shm_mgr::stop_shm();
}

void ModuleShm::OnFrame(uint64_t qwTm)
{
    if (_tmGC.On(get_curr_time()))
    {
        shm_mgr::main_loop();
    }
}
