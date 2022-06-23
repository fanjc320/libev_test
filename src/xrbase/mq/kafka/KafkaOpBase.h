#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include "log/LogMgr.h"
#include <functional>
#include "dtype.h"

typedef std::function<int32_t(const char*, size_t)>    FUN_HASH_PARTION;
typedef std::function<void(const char*, size_t)>       FUN_MSG_RECV;
typedef std::function<void(const std::string& errstr)> FUN_EVENT_ERROR;

namespace RdKafka
{
    class EventCb;
    class RebalanceCb;//GROUP�������°�֪ͨ
    class Conf;
    class PartitionerCb;//����������
    class Producer; //��������
    class Topic;
    class Consumer;//��������
    class ConsumeCb;//��������
    class KafkaConsumer;//��������
};

class KafkaOpBase
{
public:
    virtual void OnClose(const std::string& errstr);
    virtual void SetTopicName(const std::string& topic) { _topicname = topic; }

protected:
    //broker����
    std::string         _brokers = "";
    //�¼�����
    RdKafka::EventCb* _ex_event_cb = nullptr;
    //GROUP�������°�֪ͨ
    RdKafka::RebalanceCb* _ex_rebalance_cb = nullptr;

    RdKafka::Conf* _conf = nullptr;
    RdKafka::Conf* _tconf = nullptr;
    //����topic����
    std::string              _topicname;

#define CHECKERROR(A)  \
    if ((A) != RdKafka::Conf::CONF_OK)\
    {\
        LOG_ERROR("%s", errstr.c_str());\
        errstr.clear();\
    }

private:
    time_t _tmErrorPre = 0;
};
