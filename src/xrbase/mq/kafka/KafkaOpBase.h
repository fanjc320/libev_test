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
    class RebalanceCb;//GROUP消费重新绑定通知
    class Conf;
    class PartitionerCb;//分配规则控制
    class Producer; //单生产者
    class Topic;
    class Consumer;//单消费者
    class ConsumeCb;//单消费者
    class KafkaConsumer;//组消费者
};

class KafkaOpBase
{
public:
    virtual void OnClose(const std::string& errstr);
    virtual void SetTopicName(const std::string& topic) { _topicname = topic; }

protected:
    //broker名字
    std::string         _brokers = "";
    //事件处理
    RdKafka::EventCb* _ex_event_cb = nullptr;
    //GROUP消费重新绑定通知
    RdKafka::RebalanceCb* _ex_rebalance_cb = nullptr;

    RdKafka::Conf* _conf = nullptr;
    RdKafka::Conf* _tconf = nullptr;
    //单个topic名字
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
