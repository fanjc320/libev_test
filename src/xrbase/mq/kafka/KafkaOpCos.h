#pragma once

//////////////////////////////////////////////////////////////////////////
//������
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

    //����Topic������partitions����
    //--------------------------------------------------------------------
    bool ReInitConsumer();
    int  Consume();
    //--------------------------------------------------------------------

    //group����
    //--------------------------------------------------------------------
    void AddGroupTopic(const std::string& szTopic);
    bool ReInitConsumerGroup();

    //������Ѳ�ͬ�ķ���
    bool ConsumeGroup();

    //--------------------------------------------------------------------
protected:
    RdKafka::ConsumeCb* _ex_consume_cb = nullptr;
    FUN_MSG_RECV         _fn_msg_func = nullptr;

    //��������
    RdKafka::Consumer* _consumer = nullptr;
    RdKafka::Topic* _topicconsumer = nullptr;
    bool                _bInitConsumer = false;

    //��������
    RdKafka::KafkaConsumer* _consumergroup = nullptr;
    bool                    _bInitConsumerGroup = false;

    //���topic������,������
    std::vector<std::string> _topics;

    //�������ɣ���������
    uint32_t                _partition = 0;
};
