
#include "pch.h"
#include <stdio.h>
#include "shm/shm_conf.h"
#include "text/IniOp.h"
#include "string/str_format.h"
#include "log/LogMgr.h"

#define RTCONF_DEBUG   "DEBUG"
#define RTCONF_RELEASE "RELEASE"

namespace shm_mgr
{
    struct SHMCONF
    {
        char file_name[1024];
        char sec_name[256];
    };

    static SHMCONF rtconf;

    int32_t shm_conf_init(const char* filename, bool use_debug)
    {
        strncpy_safe(rtconf.file_name, filename, strlen(filename));
        const char* sec_name = use_debug ? RTCONF_DEBUG : RTCONF_RELEASE;
        strncpy_safe(rtconf.sec_name, sec_name, strlen(sec_name));
        return 0;
    }

    int32_t shm_conf_get_int(const char* key)
    {
        int32_t val = 0;
        IniOp ini;
        EIniRes ret = ini.OpenFile(rtconf.file_name);
        if (ret == EIniRes::OPENFILE_ERROR)
        {
            LOG_ERROR("tini_read err, err is %d, file name : %s, sec name : %s, key %s.\n", (int) ret, rtconf.file_name, rtconf.sec_name, key);
            return -1;
        }
        ret = ini.GetInt(rtconf.sec_name, key, val);
        if (ret == EIniRes::NO_ATTR)
        {
            LOG_ERROR("tini_read err, err is %d, file name : %s, sec name : %s, key %s.\n", (int) ret, rtconf.file_name, rtconf.sec_name, key);
            return -1;
        }
        return val;
    }

    int64_t shm_conf_get_long(const char* key)
    {
        int64_t val = 0;
        IniOp ini;
        EIniRes ret = ini.OpenFile(rtconf.file_name);
        if (ret == EIniRes::OPENFILE_ERROR)
        {
            LOG_ERROR("tini_read err, err is %d, file name : %s, sec name : %s, key %s.\n", ret, rtconf.file_name, rtconf.sec_name, key);
            return -1;
        }
        ret = ini.GetLong(rtconf.sec_name, key, val);
        if (ret == EIniRes::NO_ATTR)
        {
            LOG_ERROR("tini_read err, err is %d, file name : %s, sec name : %s, key %s.\n", ret, rtconf.file_name, rtconf.sec_name, key);
            return -1;
        }
        return val;
    }
}