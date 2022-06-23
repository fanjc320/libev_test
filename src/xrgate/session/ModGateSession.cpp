#include "pch.h"
#include "ModGateSession.h"
#include "GateHandle.h"
#include "XrGateApp.h"
#include "GatePlayerMgr.h"

bool ModGateSession::LogicInit()
{
    LogicThread* pLogicThread = XrGateApp::GetInstance()->GetLogicThread();
    ERROR_LOG_EXIT0(pLogicThread != nullptr);
    LOG_INFO("LogicInit success");
    ERROR_LOG_EXIT0(GateHandle::GetInstance()->LogicInit(pLogicThread));
    return true;
Exit0:
    return false;
}

void ModGateSession::LogicFini()
{
    GateHandle::GetInstance()->Fini();
    GatePlayerMgr::GetInstance()->Fini();
}

void ModGateSession::OnFrame(uint64_t qwTm)
{
    GateHandle::GetInstance()->OnFrame(qwTm);
    GatePlayerMgr::GetInstance()->OnFrame(qwTm);
}

void ModGateSession::OnTick(uint64_t qwTm)
{
    GateHandle::GetInstance()->OnTick(qwTm);
}
