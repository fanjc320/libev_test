
#pragma once

#include "shm/shm_def.h"
#include "shm/shm_struct_func.h"

#include <stdint.h>
#include <string>

typedef void (*FUNC_STRUCT_RESUME)(void* data);

namespace shm_mgr
{
    struct SHM_MGR;
    extern SHM_MGR*     get_shm_mgr();
    extern BOOL         is_resume();
    extern const char*  get_shm_name();

    extern int      init_shm(const char* name, bool is_resume, bool check_resume_freq);
    extern int      fini_shm();
    extern int      stop_shm();
    extern int      start_release(const char* name);

    extern int      init_struct_by_conf(int32_t shm_type, int32_t item_size, FUNC_STRUCT_RESUME resume_func = nullptr, bool pool0_alloc = true);
    extern void*    init_singleton(int32_t shm_type, int32_t item_size);
    extern bool     struct_is_inited(int32_t shm_type);
    extern int      main_loop();

    extern uint64_t alloc_x(int shm_type, void** ptr = nullptr);
    extern bool     can_alloc(int32_t shm_type);
    extern bool     can_alloc(int32_t shm_type, int64_t alloc_num);
    extern int      free_x(uint64_t mid);
    
    extern void*    mid2ptr(uint64_t mid);
    extern void*    idx2ptr(int32_t shm_type, int64_t idx);
    extern uint64_t idx2mid(int32_t shm_type, int64_t idx);
    extern int32_t  mid2type(uint64_t mid);
    extern bool     is_valid_mid(uint64_t mid);
    extern void     set_last_timer(int32_t timer_type);
    extern void     set_last_msg(int32_t from_id, uint16_t msg_id); 
    extern void     delete_file_all(const char* name);
}
