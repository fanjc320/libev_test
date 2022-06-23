/*
 *
 *      Author: venture
 */
#pragma once

#include "dtype.h"
#include "net/NetSetting.h"
#include "net/NetBase.h"
#include "memory/malloc_x.h"
#include "net/MsgHead.h"

#pragma pack(push)
#pragma pack(1)

struct ConnMsgBody
{
    EConnState		state = EConnState::UNKNOWN;
    session_oid_t	soid = INVALID_SESSION_OID;
    uint32_t		maxBuffer = 0;
};

struct AcceptConnMsg
{
    SOCKET_T        fd = -1;
    EConnType		connType = EConnType::ACCEPTED;
    session_oid_t	soid = INVALID_SESSION_OID;

    uint32_t        maxBuffer = 0;
    sockaddr        addr;
};

struct ConnMsg
{
    HostHd hd;
    ConnMsgBody body;
};


const uint32_t NET_CONN_SIZE = sizeof(ConnMsg);
const uint32_t NET_ACCEPT_SIZE = sizeof(AcceptConnMsg);

#pragma pack( pop )

class  NetConn;
class  MsgQueue;
class  ListenThread;

class IoThreadControl;

int io_notify_logic_conn_msg(NetConn* pConn, EConnState state, session_oid_t soid);
int logic_notify_io_conn_msg(IoThreadControl* pThreadCtl, const HostHd& hd
    , EConnState state, session_oid_t soid, uint32_t maxBuffer);

int logic_notify_io_conn_msg(IoThreadControl* pThreadCtl, EConnState state);
int logic_notify_listen_conn_msg(ListenThread* pListen, EConnState state);

ConnMsg* conn_msg_alloc();
AcceptConnMsg* acc_conn_alloc();
