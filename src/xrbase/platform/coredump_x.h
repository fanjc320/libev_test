/*
 *
 *      Author: venture
 */
#pragma once
#include "platform/SignalMgr.h"
#include <functional>

// Function name   : InstallCoreDumper
// Description     : ��װδ�����쳣������������dump�ļ�
extern void init_core_dumper();
extern bool set_console_info(FUNC_SIGNAL_HANDLE fnReload, FUNC_SIGNAL_HANDLE fnStop);
