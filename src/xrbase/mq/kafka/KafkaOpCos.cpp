#include "pch.h"
#include "rdkafkacpp.h"

#include "KafkaOpCos.h"
#include "KafkaOpCb.h"

KafkaOpCos::KafkaOpCos(const std::string& szTopic)
{
    _topicname = szTopic;
    _topics.push_back(szTopic);
}

KafkaOpCos::KafkaOpCos()
{

}

void KafkaOpCos::SetTopicName(const std::string& topic)
{
    _topicname = topic;
    _topics.push_back(topic);
}

KafkaOpCos::~KafkaOpCos()
{
    if (_conf != nullptr)
        delete _conf;
    if (_tconf != nullptr)
        delete _tconf;

    if (_consumer != nullptr && _topicconsumer != nullptr)
        _consumer->stop(_topicconsumer, _partition);
    if (_topicconsumer != nullptr)
        delete _topicconsumer;
    if (_consumer != nullptr)
        delete _consumer;

    if (_consumergroup != nullptr)
        delete _consumergroup;
    if (_ex_event_cb != nullptr)
        delete _ex_event_cb;
    if (_ex_rebalance_cb != nullptr)
        delete _ex_rebalance_cb;

    _conf = nullptr;
    _tconf = nullptr;

    _consumer = nullptr;
    _topicconsumer = nullptr;
    _consumergroup = nullptr;
    _ex_event_cb = nullptr;
    _ex_rebalance_cb = nullptr;
}

bool KafkaOpCos::InitConf(const std::string& brokers, FUN_MSG_RECV fnMsg)
{
    _brokers = brokers;

    _conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    _tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    std::string errstr;
    if (_conf->set("group.id", "gundam", errstr) != RdKafka::Conf::CONF_OK)
    {
        LOG_ERROR("KafkaOpCos set group.id error!");
        return false;
    }
    _conf->set("metadata.broker.list", brokers, errstr);

    _ex_event_cb = new OpEventCb();
    ((OpEventCb*) (_ex_event_cb))->InitFn(std::bind(&KafkaOpCos::OnClose, this, std::placeholders::_1));

    _ex_rebalance_cb = new OpRebalanceCb();
    CHECKERROR(_conf->set("event_cb", _ex_event_cb, errstr));
    CHECKERROR(_conf->set("rebalance_cb", _ex_rebalance_cb, errstr));

    CHECKERROR(_conf->set("batch.num.messages", "200", errstr));
    CHECKERROR(_conf->set("queue.buffering.max.ms", "5", errstr));
    CHECKERROR(_conf->set("queue.buffering.max.messages", "100000", errstr));
    CHECKERROR(_conf->set("compression.codec", "snappy", errstr));

    _ex_consume_cb = new OpConsumeCb;
    ((OpConsumeCb*) _ex_consume_cb)->InitFunc(fnMsg);

    _fn_msg_func = fnMsg;
    LOG_INFO("KafkaOpCos InitConf Success host:%s | topic: %s | partition: %u"
        , brokers.c_str(), _topicname.c_str(), _partition);

    return true;
}

bool KafkaOpCos::ReInitConsumer()
{
    if (_bInitConsumer) return true;

    if (_consumer != nullptr)
    {
        _consumer->stop(_topicconsumer, _partition);
        _consumer->poll(10);

        if (_topicconsumer != nullptr)
        {
            delete _topicconsumer;
            _topicconsumer = nullptr;
        }

        delete _consumer;
        _consumer = nullptr;
    }

    std::string errstr;

    _consumer = RdKafka::Consumer::create(_conf, errstr);
    if (!_consumer)
    {
        LOG_ERROR("KAFKA Failed to create consumer: %s", errstr.c_str());
        return false;
    }

    LOG_INFO("Created consumer: %s : %u", _consumer->name().c_str(), _partition);

    _topicconsumer = RdKafka::Topic::create(_consumer, _topicname,
        _tconf, errstr);
    if (!_topicconsumer)
    {
        LOG_ERROR("KAFKA Failed to create topic: %s", errstr.c_str());
        return false;
    }
    RdKafka::ErrorCode resp = _consumer->start(_topicconsumer, _partition, RdKafka::Topic::OFFSET_END);
    if (resp != RdKafka::ERR_NO_ERROR)
    {
        LOG_ERROR("KAFKA Failed to start consumer: %s", RdKafka::err2str(resp).c_str());
        return false;
    }

    LOG_INFO("Created consumer: %s, topic: %s"
        , _consumer->name().c_str(), _topicname.c_str());

    _bInitConsumer = true;
    return true;
}

bool KafkaOpCos::ReInitConsumerGroup()
{
    if (_bInitConsumerGroup) return true;
    if (_consumergroup != nullptr)
    {
        _consumergroup->poll(10);
        delete _consumergroup;
        _consumergroup = nullptr;
    }

    std::string errstr;
    _consumergroup = RdKafka::KafkaConsumer::create(_conf, errstr);
    if (!_consumergroup)
    {
        LOG_ERROR("KAFKA Failed to create consumer: %s", errstr.c_str());
        return false;
    }

    LOG_INFO("KAFKA Created consumer: %s", _consumergroup->name().c_str());
    RdKafka::ErrorCode err = _consumergroup->subscribe(_topics);
    if (err)
    {
        LOG_ERROR("KAFKA Failed to subscribe to topics: %s", RdKafka::err2str(err).c_str());
        return false;
    }

    _bInitConsumerGroup = true;
    return true;
}

void KafkaOpCos::OnClose(const std::string& errstr)
{
    //     _bInitConsumer = false;
    //     _bInitConsumerGroup = false;
    KafkaOpBase::OnClose(errstr);
}

int KafkaOpCos::Consume()
{
    if (!ReInitConsumer()) return -1;

    static int use_ccb = 1;
    int nlen = _consumer->consume_callback(_topicconsumer, _partition, 10, _ex_consume_cb, &use_ccb);
    return nlen;
}

void KafkaOpCos::AddGroupTopic(const std::string& szTopic)
{
    auto itr = std::find(_topics.begin(), _topics.end(), szTopic);
    if (itr != _topics.end())
    {
        return;
    }
    _topics.push_back(szTopic);
}

bool KafkaOpCos::ConsumeGroup()
{
    if (!ReInitConsumerGroup()) return false;

    bool bRet = false;
    RdKafka::Message* retmsg = _consumergroup->consume(10);
    if (retmsg != nullptr)
    {
        switch (retmsg->err())
        {
            case RdKafka::ERR_NO_ERROR:
                if (_fn_msg_func != nullptr)
                    _fn_msg_func(static_cast<const char*>(retmsg->payload()), retmsg->len());
                bRet = true;
                break;
            case RdKafka::ERR__TIMED_OUT:
            case RdKafka::ERR__PARTITION_EOF:
            case RdKafka::ERR__UNKNOWN_TOPIC:
            case RdKafka::ERR__UNKNOWN_PARTITION:
                bRet = false;
                break;
            default:
                break;
        }
    }
    else
    {
        return false;
    }
    delete retmsg;
    return bRet;
}
