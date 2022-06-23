/*
 *
 *      Author: venture
 */
#pragma once
#include "platform/SignalMgr.h"
#include <functional>

// Function name   : InstallCoreDumper
// Description     : 安装未处理异常处理器，创建dump文件
extern void init_core_dumper();
extern bool set_console_info(FUNC_SIGNAL_HANDLE fnReload, FUNC_SIGNAL_HANDLE fnStop);
