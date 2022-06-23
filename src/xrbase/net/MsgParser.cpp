/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "MsgParser.h"
#include "NetInclude.h"
#include "log/LogMgr.h"
#include "base/IoThread.h"
#include "evt2/NetEventBase.h"
#include "evt2/NetBufferEv.h"
#include "base/IoThreadControl.h"
#include "base/NetConn.h"
#include "base/IoThread.h"
#include "base/MsgQueue.h"
#include "base/EventThread.h"
#include "base/NetConnPool.h"
#include "base/MsgConn.h"

NetMsg* net_msg_alloc(uint32_t nsize)
{
    uint8_t* p = (uint8_t*) msg_alloc(nsize);
    if (p == nullptr) return nullptr;

    NetMsg* pMsg = new (p)NetMsg();
    return pMsg;
}

MsgHead* make_head_msg_to_buf(const char* pBuf, uint32_t size
    , char* arrBuffer, msg_id_t u8Module, msg_id_t u8Proto
    , uint32_t uiSeqID, uint16_t usCode)
{
    uint32_t len = MSG_HEAD_SIZE + size;
    if (len > MSG_MAX_LEN)
    {
        LOG_ERROR("msg size is too long! module:%u, protol:%u, len:%u"
            , u8Module, u8Proto, len);
        return nullptr;
    }

    MsgHead* pMsgHead = (MsgHead*) (arrBuffer);
    if (pMsgHead == nullptr) return nullptr;

    pMsgHead->mod_id = u8Module;
    pMsgHead->proto_id = u8Proto;
    pMsgHead->code = usCode;
    pMsgHead->seq_id = uiSeqID;
    pMsgHead->flag = HEAD_FLAG_BASE;
    pMsgHead->size = (uint32_t) len;

    char* pBegin = arrBuffer + MSG_HEAD_SIZE;

    //只保存头的，直接返回
    if (pBuf == nullptr || size == 0)
        return pMsgHead;

    //相同的地址的，直接返回
    if (pBegin == pBuf) return pMsgHead;

    memcpy_safe(pBegin, size, pBuf, size);
    return pMsgHead;
}

NetMsg* make_net_msg(const char* pBuf, uint32_t size, msg_id_t u8Module,
    msg_id_t u8Proto, uint32_t uiSeqID, uint16_t usCode, uint16_t coid_count)
{
    NetMsg* pNetMsg = make_net_msg(size, u8Module, u8Proto, uiSeqID, usCode, coid_count);
    if (pNetMsg == nullptr) return nullptr;

    //只保存头的，直接返回
    if (pBuf == nullptr || size == 0)
        return pNetMsg;

    MsgHead* pMsgHead = (MsgHead*) (pNetMsg->body);
    memcpy_safe(pMsgHead, size, pBuf, size);
    return pNetMsg;
}

NetMsg* make_net_msg(uint32_t size, msg_id_t u8Module, msg_id_t u8Proto, 
    uint32_t uiSeqID, uint16_t usCode, uint16_t coid_count)
{
    uint32_t len = MSG_HEAD_SIZE + size;
    uint32_t len_alloc = len + sizeof(conn_oid_t) * coid_count + NET_HOST_SIZE;
    if (len > MSG_MAX_LEN)
    {
        LOG_ERROR("msg size is too long! module:%u, protol:%u, len:%u"
            , u8Module, u8Proto, len);
        return nullptr;
    }

    NetMsg* pNetMsg = net_msg_alloc(len_alloc);
    if (pNetMsg == nullptr)
    {
        LOG_ERROR("make_head_msg alloc error!");
        return nullptr;
    }

    MsgHead* pMsgHead = (MsgHead*) (pNetMsg->body);

    pNetMsg->hd.connOid = coid_count;
    pNetMsg->hd.type = EHostMtType::MUL_NET;
    pNetMsg->hd.threadOid = -1;
    pNetMsg->size = len_alloc;

    pMsgHead->mod_id = u8Module;
    pMsgHead->proto_id = u8Proto;
    pMsgHead->code = usCode;
    pMsgHead->seq_id = uiSeqID;
    pMsgHead->flag = HEAD_FLAG_BASE;
    pMsgHead->size = (uint32_t) len;

    return pNetMsg;
}

bool fill_net_msg(thread_oid_t toid, const conn_oid_t* pCoids,
    uint16_t coid_count, NetMsg* pNetMsg)
{
    MsgHead* pNetHead = (MsgHead*)(pNetMsg->body);
    if (coid_count == 1)
    {
        pNetMsg->hd.type = EHostMtType::NET;
        pNetMsg->hd.threadOid = toid;
        pNetMsg->hd.connOid = *pCoids;
        return true;
    }
    else
    {
        MsgHead* pNetHead = nullptr;
        if (pNetMsg->hd.type == EHostMtType::NET || (pNetMsg->hd.type == EHostMtType::MUL_NET && pNetMsg->hd.connOid < coid_count))
        {
            uint32_t wlen = 0;
            if (pNetMsg->hd.type == EHostMtType::MUL_NET)
                wlen= pNetMsg->size + (coid_count - pNetMsg->hd.connOid) * sizeof(conn_oid_t);
            else
               wlen = pNetMsg->size + coid_count * sizeof(conn_oid_t);

            NetMsg* pNetNew = (NetMsg*)msg_relloc(pNetMsg, wlen);
            if (pNetNew == nullptr)
            {
                LOG_ERROR("fill_thread_conn_id alloc error!");
                return false;
            }
            pNetNew->size = wlen;
            pNetMsg = pNetNew;
            pNetHead = (MsgHead*) (pNetMsg->body);
        }
        else if (pNetMsg->hd.type == EHostMtType::MUL_NET && pNetMsg->hd.connOid > coid_count)
        {
            pNetHead = (MsgHead*) (pNetMsg->body);
        }

        if (pNetHead != nullptr)
        {
            pNetMsg->hd.type = EHostMtType::MUL_NET;
            pNetMsg->hd.threadOid = toid;
            pNetMsg->hd.connOid = coid_count;

            conn_oid_t* pDstCoids = (conn_oid_t*) (pNetMsg->body + pNetHead->size);
            memcpy(pDstCoids, pCoids, sizeof(conn_oid_t) * coid_count);
            return true;
        }
        return false;
    }
    return false;
}

NetMsg* make_mutil_net_msg(thread_oid_t toid, const conn_oid_t* pCoids
    , uint16_t coid_count, const MsgHead* pMsgHead)
{
    NetMsg* pNetMsg = make_mutil_net_msg(pMsgHead, coid_count);
    if (pNetMsg == nullptr) return nullptr;

    MsgHead* pMyMsg = MSG_HEAD_FROM_NET(pNetMsg);
    if (coid_count == 1)
    {
        pNetMsg->hd.type = EHostMtType::NET;
        pNetMsg->hd.threadOid = toid;
        pNetMsg->hd.connOid = *pCoids;
        return pNetMsg;
    }
    else
    {
        pNetMsg->hd.type = EHostMtType::MUL_NET;
        pNetMsg->hd.threadOid = toid;
        pNetMsg->hd.connOid = coid_count;

        conn_oid_t* pDstCoids = (conn_oid_t*)(pNetMsg->body + pMsgHead->size);
        memcpy(pDstCoids, pCoids, sizeof(conn_oid_t) * coid_count);
    }

    return pNetMsg;
}

NetMsg* make_mutil_net_msg(thread_oid_t toid, const conn_oid_t coid_id, const MsgHead* pMsgHead)
{
    return make_mutil_net_msg(toid, &coid_id, 1, pMsgHead);
}

NetMsg* make_mutil_net_msg(const MsgHead* pMsgHead, uint16_t coid_count)
{
    if (coid_count == 0 || pMsgHead == nullptr)
    {
        return nullptr;
    }

    if (pMsgHead->size > MSG_MAX_LEN)
    {
        LOG_ERROR("msg size is too long! module:%u, protol:%u, len:%u"
            , pMsgHead->mod_id, pMsgHead->proto_id, pMsgHead->size);
        return nullptr;
    }

    NetMsg* pHostMsg = nullptr;
    MsgHead* pMyMsg = nullptr;

    uint32_t nSize = (uint32_t) NET_HOST_SIZE + (uint32_t) (pMsgHead->size);
    if (coid_count == 1)
    {
        pHostMsg = net_msg_alloc(nSize);
        if (pHostMsg == nullptr) return nullptr;

        pHostMsg->hd.type = EHostMtType::NET;
        pHostMsg->hd.threadOid = -1;
        pHostMsg->hd.connOid = 1;
        pHostMsg->size = nSize;

        pMyMsg = (MsgHead*) pHostMsg->body;
        memcpy(pMyMsg, pMsgHead, pMsgHead->size);
    }
    else
    {
        pHostMsg = net_msg_alloc(nSize + sizeof(conn_oid_t) * coid_count);
        if (pHostMsg == nullptr) return nullptr;

        pHostMsg->hd.type = EHostMtType::MUL_NET;
        pHostMsg->hd.threadOid = 0;
        pHostMsg->hd.connOid = coid_count;
        pHostMsg->size = nSize + sizeof(conn_oid_t) * coid_count;

        pMyMsg = (MsgHead*) (pHostMsg->body);
        memcpy(pMyMsg, pMsgHead, pMsgHead->size);
    }
    return pHostMsg;
}

bool send_conn_msg_x(IoThreadControl* pThreadCtl, thread_oid_t toid
    , conn_oid_t coid, const MsgHead* pMsg)
{
    return multicast_conn_msg_x(pThreadCtl, toid, &coid, 1, pMsg);
}

bool send_conn_msg(IoThreadControl* pThreadCtl, thread_oid_t toid,
    conn_oid_t coid, NetMsg* pMsg)
{
    return multicast_conn_msg(pThreadCtl, toid, &coid, 1, pMsg);
}

bool multicast_conn_msg_x(IoThreadControl* pThreadCtl, thread_oid_t toid
    , const conn_oid_t* pCoids, uint16_t count, const MsgHead* pMsg)
{
    if (pThreadCtl == nullptr || toid == INVALID_THREAD_OID || count == 0)
    {
        return false;
    }
    NetMsg* pNetMsg = make_mutil_net_msg(toid, pCoids, count, pMsg);
    if (pNetMsg == nullptr)
    {
        return false;
    }
    if (!pThreadCtl->SendMsgHton(pNetMsg))
    {
        MSG_FREE(pNetMsg);
        return false;
    }
    return true;
}

bool multicast_conn_msg(IoThreadControl* pThreadCtl, thread_oid_t toid, 
    const conn_oid_t* pCoids, uint16_t count, NetMsg* pNetMsg)
{
    ERROR_LOG_EXIT0(pThreadCtl != nullptr);
    ERROR_LOG_EXIT0(toid != INVALID_THREAD_OID && count != 0);
    ERROR_LOG_EXIT0(fill_net_msg(toid, pCoids, count, pNetMsg));
    ERROR_LOG_EXIT0(pThreadCtl->SendMsgHton(pNetMsg));
    return true;
Exit0:
    MSG_FREE(pNetMsg);
    return false;
}

EParseResult io_recv_msg_x(NetConn* pConn, MsgQueue* pInputQueue)
{
    NetBufferEv* pBuffer = pConn->GetEvBuffer();
    if (pBuffer == nullptr) return EParseResult::BUFFER_ERROR;

    for (;;)
    {
        NetEvBuffer* pInputBuf = pBuffer->GetInput();
        if (pInputBuf->GetLength() < MSG_HEAD_SIZE)
            return EParseResult::OK;

        MsgHead msgHd;
        if (pInputBuf->CopyOut(&msgHd, MSG_HEAD_SIZE) != MSG_HEAD_SIZE)
            return EParseResult::BUFFER_ERROR;

        if ((msgHd.flag >> 12) != 0x8)
            return EParseResult::BUFFER_ERROR;

        if (msgHd.size > MSG_MAX_LEN || msgHd.size < MSG_HEAD_SIZE)
            return EParseResult::INVALID_SIZE;

        size_t msgSize = msgHd.size;
        if (pInputBuf->GetLength() < msgSize)
            return EParseResult::OK;

        if (pConn->GetParseCode() != EParseResult::OK)
            return pConn->GetParseCode();

        if (!pConn->AddRecvSec())
            return EParseResult::MORE_FAILED;

        uint32_t nAlloc = (uint32_t) NET_HOST_SIZE + (uint32_t) msgSize;
        NetMsg* pHostMsg = net_msg_alloc(nAlloc);
        if (pHostMsg == nullptr) return EParseResult::PUSH_FAILED;

        pHostMsg->hd.type = EHostMtType::NET;
        pHostMsg->hd.threadOid = pConn->GetThreadId();
        pHostMsg->hd.connOid = pConn->GetConnId();
        pHostMsg->size = nAlloc;

        size_t readRm = pBuffer->Read(&pHostMsg->body, msgSize);
        if (readRm != msgSize)
        {
            MSG_FREE(pHostMsg);
            return EParseResult::BUFFER_ERROR;
        }

        if (!pInputQueue->Push(pHostMsg))
        {
            LOG_ERROR("error!");
            MSG_FREE(pHostMsg);
            return EParseResult::PUSH_FAILED;
        }
    }

    return EParseResult::OK;
}

void io_send_msg_x(const void* pMsg, void* args)
{
    NetMsg* pNetMsg = (NetMsg*) pMsg;
    const HostHd& hd = pNetMsg->hd;

    IoThread* pIoThread = (IoThread*) args;
    if (hd.threadOid != pIoThread->EvThread()->Id())
    {
        LOG_ERROR("send_msg_hton failed(thread not match %u)", (uint32_t) hd.threadOid);
        return;
    }
    NetConnPool* pConnPool = pIoThread->ConnPool();
    if (pConnPool == nullptr)
    {
        LOG_ERROR("io_thread_conn_pool return nullptr");
        return;
    }

    if (hd.type == EHostMtType::NET)
    {
        MsgHead* pHeadMsg = (MsgHead*) (pNetMsg->body);
        if (pConnPool->Write(hd.connOid, pHeadMsg, pHeadMsg->size) != 0)
        {
            //net_conn_disconn(pConnPool, hd.m_connOid);
            //net_conn_del(pConnPool, hd.m_connOid);
            LOG_WARNING("huge(conn_oid=%u), size:%u"
                , hd.connOid, pHeadMsg->size);
        }
    }
    else if (pNetMsg->hd.type == EHostMtType::MUL_NET)
    {
        uint16_t nConnCount = hd.connOid;
        MsgHead* pHeadMsg = (MsgHead*) (pNetMsg->body);
        conn_oid_t* pCoids = (conn_oid_t*) (pNetMsg->body + pHeadMsg->size);

        for (uint16_t i = 0; i < nConnCount; ++i)
        {
            conn_oid_t coid = pCoids[i];
            if (pConnPool->Write(coid, pHeadMsg, pHeadMsg->size) != 0)
            {
                //net_conn_disconn(pConnPool, hd.m_connOid);
                //net_conn_del(pConnPool, hd.m_connOid);
                LOG_WARNING("huge(conn_oid=%u), size:%u"
                    , coid, pHeadMsg->size);
            }
        }
    }

}
