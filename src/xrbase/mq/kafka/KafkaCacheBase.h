#pragma once
#include <string>
#include <vector>
#include "net/base/MsgQueue.h"

#include "dtype.h"
#include "mq/kafka/KafkaOpCos.h"
#include "mq/kafka/KafkaOpPdu.h"
using namespace std;

class KafkaCachePush : public KafkaOpPdu
{
public:
    KafkaCachePush();
    KafkaCachePush(const std::string& szTopic);
    ~KafkaCachePush();

public:
    //key: 当多人操作同一个REDIS或者MQ时，需要每人设置一个KEY，用于唯一标识
    //partition: 当前消费者读取的分区编号
    //brokers 192.168.88.243:9092,192.168.88.242:9092
    bool InitConf(const string& brokers, FUN_HASH_PARTION fnHash = nullptr, const std::string& index = nullptr);

    bool Push(int32_t partition, const char* szmsg, uint32_t len);
    bool PushGroup(const char* szmsg, uint32_t len, const std::string& key = nullptr);

    bool Push(int32_t partition, const string& strmsg);
    bool PushGroup(const string& strmsg, const std::string& key = nullptr);

private:
    std::string  _strIndex;
};

class KafkaCachePull : public KafkaOpCos
{
public:
    KafkaCachePull();

    KafkaCachePull(const std::string& szTopic);
    ~KafkaCachePull();

public:
    //key: 当多人操作同一个REDIS或者MQ时，需要每人设置一个KEY，用于唯一标识
    //partition: 当前消费者读取的分区编号
    //brokers 192.168.88.243:9092,192.168.88.242:9092
    bool InitConf(const string& brokers, FUN_MSG_RECV fnMsg
        , int32_t partition = -1, const std::string& szIndex = nullptr);

    int MainLoop(time_t tmTick);

private:
    std::string _strIndex;
    bool _bGroup = false;
};

//支持多线程
class KafkaCachePullT
{
public:
    KafkaCachePullT();
    KafkaCachePullT(const std::string& szTopic);
    ~KafkaCachePullT();

public:
    //key: 当多人操作同一个REDIS或者MQ时，需要每人设置一个KEY，用于唯一标识
    //partition: 当前消费者读取的分区编号
    bool InitConf(const string& brokers, FUN_MSG_RECV fnMsg
        , uint32_t maxSize, NetEventBase* pEvBase, int32_t partition = -1
        , const std::string& szIndex = nullptr);
    void SetTopicName(const string& topic);

    void Stop();

private:
    void _MainLoop();
    void _OnMqRecv(const char* pMsg, size_t size);
    void _OnQueue(const event_msg_t msg);

    std::thread _thread;
    bool _exit = false;
    KafkaCachePull _pull;
    MsgQueue _queue;
    FUN_MSG_RECV _fnMsg;
};
