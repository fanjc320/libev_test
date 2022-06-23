#pragma once

#include "dtype.h"
#include "net/NetBase.h"

enum class EHostMtType : int8_t
{
    CONN = 0,
    NET = 1,
    MUL_NET = 2,
};

const uint16_t HEAD_FLAG_BASE = 0x8000;
const uint16_t HEAD_FLAG_XTRANS_LINK = 0x8001;
const uint16_t HEAD_FLAG_XTRANS_LINK_RET = 0x8002;
const uint16_t HEAD_FLAG_SVR_PING = 0x8003;
const uint16_t HEAD_FLAG_SVR_PING_RET = 0x8004;
const uint32_t MSG_MAX_LEN = 64000;

#pragma pack(push)
#pragma pack(1)

struct HostHd
{
    thread_oid_t	threadOid = INVALID_THREAD_OID;
    conn_oid_t		connOid = INVALID_CONN_OID;
    EHostMtType		type = EHostMtType::CONN;
};

struct NetMsg
{
    HostHd     hd;
    uint32_t   size = 0;
    uint8_t    body[0];
};

struct MsgHead
{
    uint32_t	size = 0;  //uint32_t wLen; 
    uint32_t    seq_id = 0;// Sequence Id
    uint16_t	flag = HEAD_FLAG_BASE;//什么类型的包
    msg_id_t	mod_id = 0;//主类型  uint8 is ok
    msg_id_t	proto_id = 0;//次类型 
    uint16_t    code = 0;  // code值,错误码
    uint8_t     body[0];
};

#define MSG_HEAD_CAST(buf)          (MsgHead*)(buf)
#define MSG_HEAD_BUF(head)	        (unsigned char*)((head)->body)
#define MSG_HEAD_SIZE(head)	        ((head)->size) - MSG_HEAD_SIZE
#define MSG_HEAD_FROM_NET(net_msg)  (MsgHead*)((net_msg)->body)

const uint32_t MSG_HEAD_SIZE = sizeof(MsgHead);
const uint32_t NET_HOST_SIZE = sizeof(NetMsg);

#pragma pack( pop )

