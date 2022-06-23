
#pragma once 
enum E_SHM_TYPE
{
    SHM_TYPE_NULL = 0,
    SHMT_TIMER_MANAGER,
    SHMT_TIMER,
    SHM_TYPE_MAX = (1 << 9) - 1,
};

namespace shm_mgr
{
    void init_type_name();
    void reg_type_name(int shm_type, const char* shm_name);
    const char* type2name(int shm_type);
    bool is_exsit_type2name(int shm_type);
}