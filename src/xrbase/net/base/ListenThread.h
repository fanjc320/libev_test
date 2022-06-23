/*
 *
 *      Author: venture
 */
#pragma once

#include <vector>
#include <thread>

#include "net/NetSetting.h"
#include "net/base/EventThread.h"
#include "net/base/MsgQueue.h"

class NetListener;
class IoThreadControl;

class ListenThread
{
public:
    ListenThread();
    ~ListenThread();

    int Init(EventThread* pLogicThread, const NetSetting& conf, IoThreadControl* pThreadCtl);
    int Start();

    int Fini();
    int Stop();

    EventThread* EvThread() { return _pEvThread; }
    IoThreadControl* Dispatcher() { return _pThreadCtl; }
    MsgQueue& OutPut();
    std::string name = "";
private:
    int _AddListen(const char* szIp, uint16_t port);
    void _OnMsgOutFun(const event_msg_t msg);

    static unsigned SP_THREAD_CALL _ListenThreadFunc(void* args);

    EventThread*        _pEvThread = nullptr;
    NetListener*        _pListener = nullptr;
    uint16_t            _port = 0;
    IoThreadControl*    _pThreadCtl = nullptr;
    MsgQueue            _OutputQueue;
    bool                _bStop = false;

};