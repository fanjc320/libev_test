/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <stdlib.h>
#include <mutex>

#include "MsgQueue.h"
#include <condition_variable>
#include "memory/malloc_x.h"
#include "MsgConn.h"
#include "log/LogMgr.h"
#include "net/NetBase.h"
#include "memory/malloc_x.h"

//////////////////////////////////////////////////////////////////////////

static void event_msgqueue_pop(void* args);

MsgQueue::MsgQueue(ECirType type)
: _type(type)
{

}
MsgQueue::~MsgQueue()
{
    Release();
}

bool MsgQueue::Init(uint32_t maxSize, NetEventBase* pEvBase, FUN_EVENT_MSG_CB cb, ECirType type)
{
	_type = type;
    _fnMsgCb = cb;

    if (_queue == nullptr)
    {
        switch (_type)
        {
        case ECirType::ONE_ONE:
            _queue = new CirQueueCas();
            break;
        case ECirType::MUL_ONE:
            _queue = new CirQueueMul();
            break;
        case ECirType::MUL_MUL:
            _queue = new CirQueueGen();
            break;
        case ECirType::ONE:
            return true;
            break;
        default:
            return false;
            break;
        }
    }

    if(!_queue->Init(maxSize))
    {
        return false;
    }
    _socketPair.name = this->name + "_sockpair";
    if (!(_socketPair.Init(pEvBase, event_msgqueue_pop, this))) 
    {
        return false;
    }

    return true;
}

void MsgQueue::Release()
{
    if (_queue != nullptr)
    {
        _queue->Release();
        delete _queue;
        _queue = nullptr;
    }

    _socketPair.Release();
    _fnMsgCb = nullptr;
}

bool MsgQueue::Push(const event_msg_t msg)
{
    if (_queue == nullptr)
    {
        if (Run(msg)) return true;
        return false;
    }
    else
    {
        int res = 0;
        res = _queue->Push(msg);
        int nTimes = 0;
        while (res == -1)
        {
            ++nTimes;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            LOG_ERROR("push max msg_queue_size, times: %d", nTimes);
            res = _queue->Push(msg);
        }
         
        if (res == 1)
        {
            LOG_MINE("mine", "", "");

            _socketPair.Write();
        }
        return true;
    }
}

bool MsgQueue::Push(const char* pMsg, uint32_t size)
{
    BufItem* pNewMsg = buf_item_alloc(size + BUF_ITEM_SIZE);
    if (pNewMsg == nullptr) return false;

    pNewMsg->size = size;
    memcpy(pNewMsg->body, pMsg, size);
    //pNewMsg->_body[size] = 0;
    if (!Push(pNewMsg))
    {
        LOG_ERROR("error!");
        MSG_FREE(pNewMsg);
        return false;
    }

	return true;
}

void MsgQueue::Pop()
{
    if (_queue == nullptr) return;
    _queue->SerPop();

    CirQueueMul::ElemP pItem = nullptr;
    pItem = _queue->Pop();

    while(pItem != nullptr)
    {
        Run(pItem);
        pItem = _queue->Pop();
    }
}

void MsgQueue::Stop()
{
    if (_queue == nullptr)
        return;

    _socketPair.Stop();
}

bool MsgQueue::Run(event_msg_t msg)
{
    if (_fnMsgCb != nullptr)
    {
        LOG_MINE("mine", "", "");

        _fnMsgCb(msg);  
    } 
    MSG_FREE(msg);
    return true;
}

static void event_msgqueue_pop(void* args)
{
    MsgQueue* pMsgQueue = (MsgQueue*)args;
    pMsgQueue->Pop();
}
