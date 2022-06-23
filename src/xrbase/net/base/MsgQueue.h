/*
 *
 *      Author: venture
 */
#pragma once

#include <mutex>
#include <functional>

#include "net/NetBase.h"
#include "net/base/CirQueueMul.h"
#include "net/base/CirQueueCas.h"
#include "net/base/CirQueueGen.h"

#include "net/base/NetSocketPair.h"

typedef void* event_msg_t;
const uint32_t EVENT_MSG_SIZE = sizeof(event_msg_t);
  
class NetEventBase;
typedef std::function<void(const event_msg_t msg)> FUN_EVENT_MSG_CB; 
class MsgQueue
{
public:
    MsgQueue(ECirType type = ECirType::ONE_ONE);
    ~MsgQueue();

    bool Init(uint32_t maxSize, NetEventBase* pEvBase, FUN_EVENT_MSG_CB cb, ECirType type);
    void Release();
    bool Push(const event_msg_t msg);
	bool Push(const char* pMsg, uint32_t size);
    void Pop();
    void Stop();
    ECirType type() { return _type; }

    bool Run(event_msg_t msg);
    std::string name = "";
private: 
    ECirType            _type = ECirType::ONE_ONE;
    CirQueue*           _queue = nullptr;
    NetSocketPair   _socketPair;
    FUN_EVENT_MSG_CB       _fnMsgCb = nullptr;
};