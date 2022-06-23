/*
 *
 *      Author: venture
 */
#pragma once

#include "log/LogMgr.h"
#include "time/TimeCount.h"

#define BeginFuncTime RdtscCount ent;
#define LeaveFuncTime LOG_INFO("used rdtcount :%lld", ent.Count());
#define ResetFuncTime ent.Reset();

