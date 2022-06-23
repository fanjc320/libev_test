#pragma once

//////////////////////////////////////////////////////////////////////////
//消费者
//////////////////////////////////////////////////////////////////////////
#include "dtype.h"
#include "mq/kafka/KafkaOpBase.h"

class KafkaOpCos : public KafkaOpBase
{
public:
    void SetTopicName(const std::string& topic);

protected:
    KafkaOpCos();
    KafkaOpCos(const std::string& szTopic);
    ~KafkaOpCos();

protected:
    //brokers 192.168.88.243:9092,192.168.88.242:9092
    bool InitConf(const std::string& brokers, FUN_MSG_RECV fnMsg);

    virtual void OnClose(const std::string& errstr);

    //单个Topic，单个partitions操作
    //--------------------------------------------------------------------
    bool ReInitConsumer();
    int  Consume();
    //--------------------------------------------------------------------

    //group操作
    //--------------------------------------------------------------------
    void AddGroupTopic(const std::string& szTopic);
    bool ReInitConsumerGroup();

    //随机消费不同的分区
    bool ConsumeGroup();

    //--------------------------------------------------------------------
protected:
    RdKafka::ConsumeCb* _ex_consume_cb = nullptr;
    FUN_MSG_RECV         _fn_msg_func = nullptr;

    //单消费者
    RdKafka::Consumer* _consumer = nullptr;
    RdKafka::Topic* _topicconsumer = nullptr;
    bool                _bInitConsumer = false;

    //组消费者
    RdKafka::KafkaConsumer* _consumergroup = nullptr;
    bool                    _bInitConsumerGroup = false;

    //多个topic组名字,组消费
    std::vector<std::string> _topics;

    //单个生成，单个消费
    uint32_t                _partition = 0;
};
