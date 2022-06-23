/*
 *
 *      Author: venture
 */
#pragma once

#include <stdint.h>
#include <functional>
#include "dtype.h"

typedef std::function<void(int32_t)> FUNC_SIGNAL_HANDLE;

struct SIGNAL_HANDLER
{
	SIGNAL_HANDLER()
	{
		nSignal = 0;
		pFunc = nullptr;
		bHasToDo = false;
	}
	int32_t				nSignal;
	FUNC_SIGNAL_HANDLE	pFunc;
	bool				bHasToDo;
};

class SignalMgr
{
public:
	SignalMgr() {};
	~SignalMgr() {};

	static SignalMgr& GetInstance();
	bool Init(void);
	bool UnInit(void);
	void MainLoop(time_t tmTick);

	bool RegSigHandler(int32_t nSignal, FUNC_SIGNAL_HANDLE pFunc);
	inline SIGNAL_HANDLER* GetSignalHandler(int32_t nSignal);

private:
	static void _CommonSignalHandleFunc(int32_t nSignal);
	bool _RegSig(int32_t nSignal);

	umap<int32_t, SIGNAL_HANDLER> _mapHandle;
	static SignalMgr					        _Instance;
};
#define g_SignalMgr SignalMgr::GetInstance()