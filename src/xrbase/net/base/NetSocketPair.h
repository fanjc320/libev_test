/*
 *
 *      Author: venture
 */
#pragma once
#include "net/NetBase.h"
//#include <string>

class NetEvent;
class NetEventBase;

//////////////////////////////////////////////////////////////////////////
typedef void(*FUN_PAIR_CALL)(void*);
class NetSocketPair
{
public:
    NetSocketPair();
    ~NetSocketPair();

    int     Write();
    void    Read();
    bool    Init(NetEventBase* pEvBase, FUN_PAIR_CALL fn, void* args);
    bool    Release();
    void    Stop();
    bool    IsStop();
    std::string name = "";
private:
    NetEventBase*   _pEvBase = nullptr;
    SOCKET_T        _fdWrite = -1;
    SOCKET_T        _fdRead = -1;
    bool            _bInit = false;
    void*           _args = nullptr;
    FUN_PAIR_CALL     _fnCall = nullptr;
    bool            _bStop = false;
    NetEvent*       _pEvent = nullptr;
    static void     _EventPop(SOCKET_T fd, short events, void* args);
    void            _DoCall();
};