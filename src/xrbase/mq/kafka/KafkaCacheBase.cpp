#include "pch.h"
#include "memory/malloc_x.h"
#include "KafkaCacheBase.h"
#include "time/time_func_x.h"

#define MAX_PICK_MSG  1000
KafkaCachePush::KafkaCachePush(const std::string& szTopic)
    : KafkaOpPdu(szTopic)
{

}

KafkaCachePush::KafkaCachePush()
{

}

KafkaCachePush::~KafkaCachePush()
{ }

bool KafkaCachePush::InitConf(const std::string& brokers, FUN_HASH_PARTION fnHash, const std::string& index)
{
    if (!index.empty())
    {
        _strIndex = index;
        _topicname = _strIndex + "-" + _topicname;
    }

    return KafkaOpPdu::InitConf(brokers, fnHash);
}

bool KafkaCachePush::PushGroup(const char* szmsg, uint32_t len, const std::string& key)
{
    return KafkaOpPdu::ProducerGroup(szmsg, len, key);
}

bool KafkaCachePush::PushGroup(const string& strmsg, const std::string& key /*= nullptr*/)
{
    return KafkaOpPdu::ProducerGroup(strmsg.c_str(), (uint32_t)strmsg.size(), key);
}

bool KafkaCachePush::Push(int32_t partition, const char* szmsg, uint32_t len)
{
    if (partition == 0) return false;
    partition = partition - 1;

    partition = partition % 100;
    return KafkaOpPdu::Producer(partition, szmsg, len);
}


bool KafkaCachePush::Push(int32_t partition, const string& strmsg)
{
    return KafkaCachePush::Push(partition, strmsg.c_str(), (uint32_t)strmsg.size());
}

KafkaCachePull::KafkaCachePull(const std::string& szTopic)
    : KafkaOpCos(szTopic)
{

}

KafkaCachePull::KafkaCachePull()
{

}

KafkaCachePull::~KafkaCachePull()
{ }

bool KafkaCachePull::InitConf(const string& brokers, FUN_MSG_RECV fnMsg
    , int32_t partition, const std::string& szIndex)
{
    if (!szIndex.empty())
    {
        _strIndex = szIndex;
        _topicname = _strIndex + "-" + _topicname;
    }
    if (partition < 0)
    {
        _bGroup = true;
        _partition = (uint32_t) partition;
    }
    else
    {
        partition = partition - 1;
        partition = partition % 100;
        _bGroup = false;
        _partition = (uint32_t) partition;
    }
    return KafkaOpCos::InitConf(brokers, fnMsg);
}

int KafkaCachePull::MainLoop(time_t tmTick)
{
    int size = 0;
    int nTemp = 0;

    if (_bGroup)
    {
        for (size_t i = 0; i < MAX_PICK_MSG; ++i)
        {
            if (!KafkaOpCos::ConsumeGroup())
                return size;
            ++size;
        }
    }
    else
    {
        for (size_t i = 0; i < MAX_PICK_MSG
            && size < MAX_PICK_MSG; ++i)
        {
            nTemp = KafkaOpCos::Consume();
            if (nTemp <= 0)
                break;
            size += nTemp;
        }
    }
    return size;
}

KafkaCachePullT::KafkaCachePullT(const std::string& szTopic)
    : _pull(szTopic)
{

}

KafkaCachePullT::KafkaCachePullT()
{

}

KafkaCachePullT::~KafkaCachePullT()
{
    _exit = true;
    if (_thread.joinable())
        _thread.join();
}

bool KafkaCachePullT::InitConf(const string& brokers, FUN_MSG_RECV fnMsg
    , uint32_t maxSize, NetEventBase* pEvBase, int32_t partition
    , const std::string& szIndex)
{
    _fnMsg = fnMsg;

    bool ret = _pull.InitConf(brokers,
        std::bind(&KafkaCachePullT::_OnMqRecv, this, std::placeholders::_1, std::placeholders::_2)
        , partition, szIndex);

    if (!ret)
    {
        LOG_ERROR("KafkaCachePullT InitConf Error %s:%s", brokers.c_str(), szIndex.c_str());
        return false;
    }

    ret = _queue.Init(maxSize, pEvBase, std::bind(&KafkaCachePullT::_OnQueue, this,
        std::placeholders::_1), ECirType::MUL_ONE);
    if (!ret)
    {
        LOG_ERROR("KafkaCachePullT InitConf Error %s:%s", brokers.c_str(), szIndex.c_str());
        return false;
    }

    _thread = std::thread(std::bind(&KafkaCachePullT::_MainLoop, this));
    return true;
}

void KafkaCachePullT::SetTopicName(const string& topic)
{
    _pull.SetTopicName(topic);
}

void KafkaCachePullT::Stop()
{
    _queue.Stop();
    _exit = true;
}

void KafkaCachePullT::_MainLoop()
{
    while (!_exit)
    {
        _pull.MainLoop(get_tick_count());
    }
}

void KafkaCachePullT::_OnMqRecv(const char* pMsg, size_t size)
{
    if (size <= 0) return;
    if (!_queue.Push(pMsg, (uint32_t)size))
    {
        LOG_ERROR("recv %d push error!", size);
    }
}

void KafkaCachePullT::_OnQueue(const event_msg_t msg)
{
    BufItem* pNewMsg = (BufItem*) msg;
    if (pNewMsg == nullptr) return;

    if (_fnMsg != nullptr)
        _fnMsg((const char*) (pNewMsg->body), (uint32_t)pNewMsg->size);
}
