#pragma once

//////////////////////////////////////////////////////////////////////////
//生产者
//////////////////////////////////////////////////////////////////////////
#include "dtype.h"
#include "mq/kafka/KafkaOpBase.h"

class KafkaOpPdu : public KafkaOpBase
{
protected:
    KafkaOpPdu();
    KafkaOpPdu(const std::string& szTopic);
    ~KafkaOpPdu();

protected:
    //组生成时，设置fnHash
    //brokers 192.168.88.243:9092,192.168.88.242:9092
    bool InitConf(const std::string& brokers, FUN_HASH_PARTION fnHash = nullptr);

    virtual void OnClose(const std::string& errstr);

    //单个Topic，单个partitions操作
    //--------------------------------------------------------------------
    bool ReInitProducer();
    bool Producer(int32_t partition, const char* szmsg, const uint32_t len);
    //--------------------------------------------------------------------

    //随机发往不同的分区
    bool ProducerGroup(const char* szmsg, const uint32_t len, const std::string& key = nullptr);

    //--------------------------------------------------------------------
protected:
    bool Producer(int32_t partition, const char* szmsg, const uint32_t len, const std::string& key);

    //分配规则控制
    RdKafka::PartitionerCb* _ex_partition_cb = nullptr;

    //单生产者
    RdKafka::Producer* _producer = nullptr;
    RdKafka::Topic* _topicproducer = nullptr;
    bool _bInitProducer = false;

    //组生产
    FUN_HASH_PARTION  _fn_hash_func = nullptr;
};