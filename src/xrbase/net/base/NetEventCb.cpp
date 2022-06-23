#include "pch.h"
#include "NetEventCb.h"
#include "NetConn.h"
#include "ReConnEvent.h"
#include "MsgQueue.h"
#include "net/evt2/NetBufferEv.h"
#include "log/LogMgr.h"
#include "NetConnPool.h"
#include "event2/bufferevent.h"
#include "event2/event.h"
#include "IoThread.h"
#include "MsgConn.h"
#include "net/MsgParser.h"
#include "net/base/sock_opt.h"

static FUN_IO_RECV_MSG g_io_recv_msg_fn = io_recv_msg_x;
void regfn_io_recv_msg(FUN_IO_RECV_MSG fn)
{
    if (fn) g_io_recv_msg_fn = fn;
}

void conn_read_cb(bufferevent* pBuffer, void* args)
{
    NetConn* pConn = (NetConn*) args;
    if (pConn == nullptr)
    {
        return;
    }

    MsgQueue& InputQueue = pConn->GetPool()->IoThreads()->InPut();
    if (g_io_recv_msg_fn == nullptr) return;

    if (pConn->GetParseCode() == EParseResult::CUS_KILL)
    {
        LOG_ERROR("has kill! ip:%s, threadid:%u, conid:%u, secrecv:%u, allrecv:%u"
            , sock_opt::Inet_Ip(pConn->GetIp()).c_str(), pConn->GetThreadId(), pConn->GetConnId()
            , pConn->GetSecRecv(), pConn->GetAllRecv());
        return;
    }

    EParseResult relt = EParseResult::OK;

    if (pConn->GetEvBuffer()->BufEv() != pBuffer)
    {
        relt = EParseResult::BUFFER_ERROR;
        pConn->SetParseCode(EParseResult::BUFFER_ERROR);
        pConn->GetPool()->DisConn(pConn->GetConnId());
    }
    else
    {
        relt = (*g_io_recv_msg_fn)(pConn, &InputQueue);
    }

    if (relt != EParseResult::OK)
    {
        pConn->SetParseCode(relt);
        LOG_ERROR("ip:%s, threadid:%u, conid:%u, secrecv:%u, parser msg failed(no=%d), allrecv:%u"
            , sock_opt::Inet_Ip(pConn->GetIp()).c_str(), pConn->GetThreadId(), pConn->GetConnId()
            , pConn->GetSecRecv(), (int32_t) relt, pConn->GetAllRecv());

        if (pConn->GetParseCode() != EParseResult::CUS_KILL)
            pConn->GetPool()->DisConn(pConn->GetConnId());
    }
}

void init_buffer_mask(class NetBufferEv* pBuffer)
{
    pBuffer->Enable(EV_READ | EV_WRITE);
    pBuffer->SetWaterMark(EV_READ, MSG_HEAD_SIZE, 128 * 1024);
}

void conn_write_cb(bufferevent* pBuffer, void* args)
{
    //     CNetConn* pConn = (CNetConn*)args;
    //     if (pConn == nullptr) 
    //     {
    //         return;
    //     }
    //     evbuffer *pOutput = bufferevent_get_output(pBuffer);
    //     if (evbuffer_get_length(pOutput) == 0) 
    //     {
    //         LOG_ERROR("ip:%s, threadid:%u, conid:%u, secrecv:%u"
    //             , inet_ip(pConn->get_ip()).c_str(), pConn->get_thread_id(), pConn->get_conn_id()
    //             , pConn->get_sec_recv());
    //     }
}

void conn_event_cb(bufferevent* pBuffer, short events, void* args)
{
    NetConn* pConn = (NetConn*) args;
    if (pConn == nullptr)
    {
        return;
    }
    if (events & BEV_EVENT_EOF)
    {
        //PRINTF(LOG_DEBUG, "Connection closed.\n", 1);
    }
    else if (events & BEV_EVENT_ERROR)
    {
        //ELOG((ELOG_ERR, "Got an error on the connection: %s\n", strerror(errno)));
    }
    else
    {
        //PRINTF(LOG_DEBUG, "Connection disconnect.\n", 1);
    }
    //LOG_ERROR("error:%d", events);
    pConn->SetParseCode(EParseResult::BUFFER_ERROR);
    pConn->GetPool()->DisConn(pConn->GetConnId());
}

void reconnect_cb(struct bufferevent* bev, short events, void* arg)
{
    ReconnEvent* pReConn = (ReconnEvent*) arg;
    pReConn->OnConnect(events);
}
