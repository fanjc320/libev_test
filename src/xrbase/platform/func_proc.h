/*
 *
 *      Author: venture
 */
#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <chrono>
#include <vector>
#include "dtype.h"
#include <sys/stat.h>
extern uint16_t  get_cpu_num();

extern int get_pid();

extern umap<int32_t, std::string> get_proc_list(const std::string& strCmd);
extern std::string run_cmd_res(const std::string& strCmd);
extern bool run_cmd(const std::string& strCmd, bool bFork = false);

extern bool is_exsit_proc(const std::string& strCmd);

extern bool is_listen_port(unsigned long dwPort, bool bTcp = true);
extern int64_t get_proc_mem();

extern std::string get_local_ip();
extern std::string get_net_ip();
extern int32_t get_local_ip_num(void);