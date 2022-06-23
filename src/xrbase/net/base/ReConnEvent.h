/*
 *
 *      Author: venture
 */
#pragma once

#include <string>

#include "net/NetBase.h"
#include "net/EventTimer.h"
#include "time/time_func_x.h"
#include "time/TimeCount.h"

class	NetBufferEv;
class	EventThread;
class	IoThreadControl;
class	ReconnSession;

class ReconnEvent
{
public:
    ReconnEvent(EventThread* pThread, IoThreadControl* pThreadCtl,
        int iConnSecs, uint32_t maxBuffer);
    virtual ~ReconnEvent();

    void OnTimer(uint64_t dwTM);
    void Connect();

    void Init(const std::string& strIp, uint16_t nPort);
    void OnConnect(short events);
    void OnDisconnect();
    bool IsConnected() const { return _bConnected; }
    void SetOwner(ReconnSession* pOwner) { _pOwner = pOwner; }
    void StopTimer();

    const std::string& GetIp() const { return _strIp; }
    void SetIp(const std::string& val) { _strIp = val; }
    uint16_t GetPort() const { return _nPort; }
    void SetPort(uint16_t val) { _nPort = val; }

private:
    void _FreeBufferEvent();

private:
    ReconnSession*      _pOwner = nullptr;
    NetBufferEv*        _pBuffer = nullptr;
    EventThread*        _pThread = nullptr;
    IoThreadControl*   _pThreadCtl = nullptr;
    std::string         _strIp;
    uint16_t            _nPort = 0;
    bool                _bConnected = false;
    bool                _bConnecting = false;
    uint32_t            _nConnectNum = 0;
    uint32_t		    _nConnectAll = 0;
    uint32_t            _maxBuffer = 0;
    GenCount            _tmrConnect;
    GenCount            _tmrPing;
    bool                _bTimerStop = false;
    sockaddr_in         _addr;
    std::string         _objName;
};
