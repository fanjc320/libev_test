/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "SvrArgOpt.h"
#include "string/str_num.h"
#include "string/str_format.h"
#include "const.h"
#include "platform/file_func.h"

void SvrArgOpt::Man()
{
    LOG_ERROR("Usage:./test -i 101");
    LOG_ERROR(" -m: start¡¢resume")
    LOG_ERROR(" -i: server no.");
    LOG_ERROR(" -a: areano.");
    LOG_ERROR(" -c: config file.");
    LOG_ERROR(" -n: alias name.");
    LOG_ERROR(" -h: help.");
}

bool SvrArgOpt::Argv(int argc, char* argv[])
{
    int32_t opt;
    while ((opt = getopt(argc, argv, "m:i:a:c:n:h")) != -1)
    {
        switch (opt)
        {
            case 'm':
                _resume = (strcmp_ncase("resume", optarg) == 0);
                _mode = optarg;
                break;
            case 'i':
                _index = stoi_x(optarg);
                break;
            case 'a':
                _areano = stoi_x(optarg);
                break;
            case 'c':
                _config = optarg;
                break;
            case 'n':
                _alias = optarg;
                break;
            case 'h':
                Man();
                return false;
                break;
            default:
                LOG_ERROR("argv error, use -h help!");
                return false;
                break;
        }
    }

    if (_areano == 0)
    {
        LOG_WARNING("areano not set default is 1");
        _areano = 1;
    }

    if (_index == 0)
    {
        LOG_WARNING("index not set default is 1");
        _index = 1;
    }

    if (_config.empty())
    {
        LOG_WARNING("config file not set, use defalut path ");
        _config = file::get_local_path(DEF_APP_CONFIG_PATH);
    }

    return true;
}