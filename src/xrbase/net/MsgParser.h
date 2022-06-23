/*
 *
 *      Author: venture
 */
#pragma once

#include "net/NetBase.h"
#include "net/MsgHead.h"

#include <functional>
class	NetConn;
class	MsgQueue;
class	LogicThread;
class   IoThreadControl;

typedef std::function<bool(const MsgHead*)> FUNC_MSG_HEAD;
extern NetMsg* net_msg_alloc(uint32_t nsize);

//发送消息
extern bool send_conn_msg_x(IoThreadControl* pThreadCtl, thread_oid_t toid
	, conn_oid_t coid, const MsgHead* pMsg);

extern bool send_conn_msg(IoThreadControl* pThreadCtl, thread_oid_t toid
    , conn_oid_t coid, NetMsg* pMsg);

//发送到多个连接上, 广播消息
extern bool multicast_conn_msg_x(IoThreadControl* pThreadCtl, thread_oid_t toid
	, const conn_oid_t* pCoids, uint16_t count, const MsgHead* pMsg);

extern bool multicast_conn_msg(IoThreadControl* pThreadCtl, thread_oid_t toid
    , const conn_oid_t* pCoids, uint16_t count, NetMsg* pMsg);

//iothread中的处理
typedef EParseResult(*FUN_IO_RECV_MSG)(NetConn* pConn, MsgQueue* pInputQueue);
typedef void (*FUN_IO_SEND_MSG)(const void* pMsg, void* args);

void regfn_io_recv_msg(FUN_IO_RECV_MSG fn);
void regfn_io_send_msg(FUN_IO_SEND_MSG fn);

extern EParseResult io_recv_msg_x(NetConn* pConn, MsgQueue* pInputQueue);
extern void         io_send_msg_x(const void* pMsg, void* args);

extern MsgHead* make_head_msg_to_buf(const char* pBuf, uint32_t size, char* arrBuffer,
            msg_id_t u8Module, msg_id_t u8Proto, uint32_t uiSeqID, uint16_t usCode);

extern NetMsg*  make_net_msg(const char* pBuf, uint32_t size, msg_id_t u8Module,
                msg_id_t u8Proto, uint32_t uiSeqID, uint16_t usCode, uint16_t coid_count);

extern NetMsg*  make_net_msg(uint32_t size, msg_id_t u8Module, msg_id_t u8Proto,
                uint32_t uiSeqID, uint16_t usCode, uint16_t coid_count);

extern bool     fill_net_msg(thread_oid_t toid, const conn_oid_t* pCoids, 
                    uint16_t coid_count, NetMsg* pMsgHead);

extern NetMsg*  make_mutil_net_msg(thread_oid_t toid, const conn_oid_t* pCoids, 
                    uint16_t coid_count, const MsgHead* pMsgHead);
extern NetMsg*  make_mutil_net_msg(thread_oid_t toid, const conn_oid_t coid_id, const MsgHead* pMsgHead);
extern NetMsg*  make_mutil_net_msg(const MsgHead* pMsgHead, uint16_t coid_count);