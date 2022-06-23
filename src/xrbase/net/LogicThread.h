/*
 *
 *      Author: venture
 */
#pragma once

#include <string>
#include "net/NetSetting.h"
#include "net/base/EventThread.h"
#include "net/base/ListenThread.h"
#include "net/base/IoThreadControl.h"
#include "net/EventTimer.h"

class	IoThreadControl;
class	ReconnEvent;
class	ReconnSession;
class	LogicHandle;
struct	NetMsg;

class LogicThread
{
protected:
    LogicThread();
    ~LogicThread();

public:
    static LogicThread* New(const NetSetting& setting, const NetSetting& recon_setting);
    static LogicThread* New();

    static void Delete(LogicThread* pThread);

    bool InitLogic();
    EventThread* EvThread();

    bool InitNetSetting(const NetSetting& setting);
    bool InitNetReconnSetting(const NetSetting& recon_setting);

    int MainLoop();
    int Stop();

    int AddReconn(ReconnSession* pSession, const std::string& strIp, uint16_t nPort, int iConnSecs, uint32_t maxBuffer);
    int RemoveReconn(ReconnSession* pSession);

    bool SendListen(NetMsg* pNetMsg);
    bool SendReconn(NetMsg* pNetMsg);

    void RegInterfaceListen(LogicHandle* pLogicHandle);
    void RegInterfaceReconn(LogicHandle* pLogicHandle);

    void OnTimer(uint64_t dwTM);

    LogicHandle* GetLogicHandle();
    LogicHandle* GetReConnLogicHandle();

    IoThreadControl* GetThreadCtl();
    IoThreadControl* GetReconnThreadCtl();

    NetSetting GetSetting() const { return _setting; }
    std::string name = "";

protected:
    void _SetSetting(const NetSetting& val) { _setting = val; }

private:
    bool _CreateListener(IoThreadControl* pIoThreadCtl, LogicThread* pThread, const NetSetting& conf);
    bool _CreateThreadCtl(IoThreadControl* pIoThreadCtl, LogicThread* pThread, const NetSetting& conf);

    EventThread*        _pEvThread = nullptr;
    IoThreadControl*    _pIoThreadCtl = nullptr;
    ListenThread*       _pListenThread = nullptr;
    IoThreadControl*    _pReConnThreadCtl = nullptr;
    EventTimer*         _pTimercon = nullptr;
    NetSetting          _setting;
    NetSetting          _reconSetting;
};