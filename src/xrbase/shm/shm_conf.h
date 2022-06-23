#pragma once
#include <stdint.h>

namespace shm_mgr
{
    int32_t shm_conf_init(const char* filename, bool use_debug);

    int32_t shm_conf_get_int(const char* key);
    int64_t shm_conf_get_long(const char* key);
}