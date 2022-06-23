#include "pch.h"
#include "ClientModuleReconn.h"
#include "ClientReconnHandle.h"
#include "ClientApp.h"
#include "ClientReconn.h"
#include "math/math_basic.h"
#include "net/LogicThread.h"
#include "ClientArgOpt.h"
#include "ClientReconnHandle.h"

bool ClientModuleReconn::LogicInit()
{
    LogicThread* pLogicThread = ClientApp::GetInstance()->GetLogicThread();
    ClientReconn* pRecon = nullptr;
    int ret = 0;
    ERROR_LOG_EXIT0(pLogicThread != nullptr);
    ERROR_LOG_EXIT0(ClientReconnHandle::GetInstance()->Init(pLogicThread));

    {
		pRecon = ClientReconn::New();
		std::string ip = ClientApp::GetClientOpt()->GetHostIp();
		int port = 19100;
		ret = pLogicThread->AddReconn(pRecon, ip, port, 5, MEM_SIZE_1M);
		if (ret != 0)
		{
			LOG_ERROR("add reconn error %s: %d!", ip.c_str(), port);
			delete pRecon;
		}
    }
	LOG_INFO("LogicInit success");

    return true;
Exit0:
    return false;

}

void ClientModuleReconn::LogicFini()
{
    
}

void ClientModuleReconn::OnFrame(uint64_t qwTm)
{
    
}

void ClientModuleReconn::OnTick(uint64_t qwTm)
{
    
}
