#include "pch.h"
#include "net/LogicThread.h"
#include "log/LogMgr.h"
#include "math/math_basic.h"
#include "platform/func_proc.h"
#include "ClientReconn.h"
#include "ClientReconnHandle.h"

bool ClientReconnHandle::Init(LogicThread* pLogicThread)
{
    NetSetting setting;
    setting.Init(get_cpu_num(), 512, MEM_SIZE_128K, MEM_SIZE_1M, MEM_SIZE_1M);
    ERROR_LOG_EXIT0(pLogicThread->InitNetReconnSetting(setting));
    pLogicThread->RegInterfaceReconn(this);

    return true;
Exit0:
    return false;
}

void ClientReconnHandle::OnFrame(uint64_t qwTick)
{
    for (auto& itr : g_mapReconn)
    {
        itr->OnFrame();
    }
}

void ClientReconnHandle::OnTick(uint64_t qwTick)
{
    for (auto& itr : g_mapReconn)
    {
        itr->OnTick();
    }
}

bool ClientReconnHandle::OnReload()
{
    return true;
}

bool ClientReconnHandle::OnStop()
{
    return true;
}

void ClientReconnHandle::Fini()
{
    for (auto& itr : g_mapReconn)
    {
        KillReconSession(itr);
    }
    for (auto& itr : g_mapReconn)
    {
        SAFE_DELETE(itr);
    }

    g_mapReconn.clear();
}