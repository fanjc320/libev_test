
#include "pch.h"
#include "shm_type.h"
#include "shm_conf.h"
#include "log/LogMgr.h"

namespace shm_mgr
{
    static const char* g_shm_name[SHM_TYPE_MAX + 1] = { 0 };
#define DECLARE_SHM_NAME(type) g_shm_name[type]= #type;

    void reg_type_name(int shm_type, const char* shm_name)
    {
        if (shm_type >= SHM_TYPE_MAX)
            return;
        g_shm_name[shm_type] = shm_name;
    }

    void init_type_name()
    {
        DECLARE_SHM_NAME(SHM_TYPE_NULL);
        DECLARE_SHM_NAME(SHMT_TIMER);
        DECLARE_SHM_NAME(SHMT_TIMER_MANAGER);
        DECLARE_SHM_NAME(SHM_TYPE_MAX);
    }

    const char* type2name(int shm_type)
    {
        if (shm_type < 0 || shm_type >= SHM_TYPE_MAX)
            return nullptr;

        const char* ret = g_shm_name[shm_type];
        if (ret == nullptr)return "";
        return ret;
    }

    bool is_exsit_type2name(int shm_type)
    {
        if (shm_type < 0 || shm_type >= SHM_TYPE_MAX)
            return false;

        const char* ret = g_shm_name[shm_type];
        if (ret == nullptr)return false;
        return true;
    }
}
