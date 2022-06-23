/*
 *
 *      Author: venture
 */
#pragma once
#include "server/mq/SvrProgMqBase.h"
#include "time/TimeCount.h"
#include "time/time_func_x.h"
#include "util/HandleEvent.h"
#include "const.h"
#include "pb/pb_parse.h"

class SvrProgMq : public SvrProgMqBase
{
public:
    SvrProgMq() = default;
    SvrProgMq(const std::string& szName, uint32_t dwType);
    SvrProgMq(const std::string& szFrom, const std::string& szTo, int32_t max_buf);
    virtual ~SvrProgMq();

    virtual bool LogicInit();

    //id: proto_id
    virtual int OnEvent(uint32_t id, const MsgHead* pMsg) = 0;
    virtual bool OnEventDefault(const MsgHead* pMsg) { return true; };

    virtual void OnFrame(uint64_t qwTm);
    virtual void OnTick(uint64_t qwTm); //“ª∫¡√Î“ª¥Œ
    virtual bool IsConnect();
    virtual void OnDisConnect();
    virtual void OnConnect();

    virtual bool OnHandleMsg(const MsgHead* pMsg);

protected:
    NumCount<60 * 1000> _logConnect;
    int32_t _send_inv = 10;
    int32_t _check_connect = 30;
};

