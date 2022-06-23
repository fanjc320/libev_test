#pragma once

//////////////////////////////////////////////////////////////////////////
//������
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
    //������ʱ������fnHash
    //brokers 192.168.88.243:9092,192.168.88.242:9092
    bool InitConf(const std::string& brokers, FUN_HASH_PARTION fnHash = nullptr);

    virtual void OnClose(const std::string& errstr);

    //����Topic������partitions����
    //--------------------------------------------------------------------
    bool ReInitProducer();
    bool Producer(int32_t partition, const char* szmsg, const uint32_t len);
    //--------------------------------------------------------------------

    //���������ͬ�ķ���
    bool ProducerGroup(const char* szmsg, const uint32_t len, const std::string& key = nullptr);

    //--------------------------------------------------------------------
protected:
    bool Producer(int32_t partition, const char* szmsg, const uint32_t len, const std::string& key);

    //����������
    RdKafka::PartitionerCb* _ex_partition_cb = nullptr;

    //��������
    RdKafka::Producer* _producer = nullptr;
    RdKafka::Topic* _topicproducer = nullptr;
    bool _bInitProducer = false;

    //������
    FUN_HASH_PARTION  _fn_hash_func = nullptr;
};