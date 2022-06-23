/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <signal.h>

#include "dtype.h"
#include "SignalMgr.h"
#include "log/LogMgr.h"

SignalMgr SignalMgr::_Instance;

bool SignalMgr::Init(void)
{
	_mapHandle.clear();
	return true;
}

bool SignalMgr::UnInit(void)
{
	_mapHandle.clear();
	return true;
}

SignalMgr& SignalMgr::GetInstance()
{
	return _Instance;
}

SIGNAL_HANDLER* SignalMgr::GetSignalHandler(int32_t nSignal)
{
	umap<int32_t, SIGNAL_HANDLER>::iterator it = _mapHandle.find(nSignal);
	ERROR_LOG_EXIT0(it != _mapHandle.end());

	return &(it->second);
Exit0:
	return nullptr;
}

bool SignalMgr::RegSigHandler(int32_t nSignal, FUNC_SIGNAL_HANDLE pFunc)
{
	int32_t nRetCode = 0;
	SIGNAL_HANDLER* pHandler = nullptr;

	umap<int32_t, SIGNAL_HANDLER>::iterator it = _mapHandle.find(nSignal);
	ERROR_LOG_EXIT0(it == _mapHandle.end());

	pHandler = &(_mapHandle[nSignal]);
	ERROR_LOG_EXIT0(pHandler);

	pHandler->nSignal = nSignal;
	pHandler->pFunc = pFunc;
	pHandler->bHasToDo = false;

	nRetCode = _RegSig(nSignal);
	ERROR_LOG_EXIT0(nRetCode);

	return true;
Exit0:
	LOG_ERROR("reg_sig_handler failed, signal %d", nSignal);
	return false;
}

void SignalMgr::MainLoop(time_t tmTick)
{
	for (auto it = _mapHandle.begin(); it != _mapHandle.end(); ++it)
	{
		SIGNAL_HANDLER& handler = it->second;
		if (!handler.bHasToDo)
			continue;

		if (handler.pFunc == nullptr)
			continue;

		handler.pFunc(it->first);
		handler.bHasToDo = false;
	}
}

void SignalMgr::_CommonSignalHandleFunc(int32_t nSignal)
{
	SIGNAL_HANDLER* pSignalHander = nullptr;

	//INF("recv signal [%d]", nSignal);

	pSignalHander = SignalMgr::GetInstance().GetSignalHandler(nSignal);
	if (pSignalHander)
	{
		pSignalHander->bHasToDo = true;
		if (pSignalHander->pFunc != nullptr)
			pSignalHander->pFunc(nSignal);
		pSignalHander->bHasToDo = false;
	}
}

bool SignalMgr::_RegSig(int32_t nSignal)
{
	int32_t nRetCode = 0;

	signal(nSignal, _CommonSignalHandleFunc);
	return true;
}

