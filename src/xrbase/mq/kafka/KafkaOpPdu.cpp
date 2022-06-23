#include "pch.h"
#include "rdkafkacpp.h"

#include "KafkaOpPdu.h"
#include "KafkaOpBase.h"
#include "KafkaOpCb.h"

KafkaOpPdu::KafkaOpPdu(const std::string& szTopic)
{
    _topicname = szTopic;
}

KafkaOpPdu::KafkaOpPdu()
{

}

KafkaOpPdu::~KafkaOpPdu()
{
    if (_conf != nullptr)
        delete _conf;
    if (_tconf != nullptr)
        delete _tconf;
    if (_topicproducer != nullptr)
        delete _topicproducer;
    if (_producer != nullptr)
        delete _producer;

    if (_ex_event_cb != nullptr)
        delete _ex_event_cb;
    if (_ex_rebalance_cb != nullptr)
        delete _ex_rebalance_cb;

    _producer = nullptr;
    _topicproducer = nullptr;
    _conf = nullptr;
    _tconf = nullptr;

    _ex_event_cb = nullptr;
    _ex_rebalance_cb = nullptr;
}

bool KafkaOpPdu::InitConf(const std::string& brokers, FUN_HASH_PARTION fnHash)
{
    _brokers = brokers;

    _conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    _tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    std::string errstr;
    //     if (_conf->set("group.id", "gundam", errstr) != RdKafka::Conf::CONF_OK) 
    //     {
    //         Log_Error("KafkaOpPdu set group.id error!");
    //         return false;
    //     }

    CHECKERROR(_conf->set("metadata.broker.list", brokers, errstr));
    _ex_event_cb = new OpEventCb();
    ((OpEventCb*) (_ex_event_cb))->InitFn(std::bind(&KafkaOpPdu::OnClose, this, std::placeholders::_1));

    _ex_rebalance_cb = new OpRebalanceCb();
    CHECKERROR(_conf->set("event_cb", _ex_event_cb, errstr));
    CHECKERROR(_conf->set("rebalance_cb", _ex_rebalance_cb, errstr));
    CHECKERROR(_conf->set("batch.num.messages", "20", errstr));
    CHECKERROR(_conf->set("queue.buffering.max.ms", "1000", errstr));
    CHECKERROR(_conf->set("queue.buffering.max.messages", "1000000", errstr));
    CHECKERROR(_conf->set("compression.codec", "gzip", errstr));
    //CHECKERROR(_conf->set("connections.max.idle.ms", "30758400000", errstr));

    //生产者特有
    CHECKERROR(_conf->set("retries", "5", errstr));
    CHECKERROR(_conf->set("retry.backoff.ms", "10000", errstr));
    //生产者特有
    CHECKERROR(_tconf->set("request.timeout.ms", "10000", errstr));
    CHECKERROR(_tconf->set("message.timeout.ms", "10000", errstr));
    CHECKERROR(_tconf->set("request.required.acks", "-1", errstr));
    CHECKERROR(_tconf->set("compression.codec", "gzip", errstr));

    if (fnHash != nullptr)
    {
        _fn_hash_func = fnHash;
        _ex_partition_cb = new OpHashPartitionerCb;
        ((OpHashPartitionerCb*) _ex_partition_cb)->InitFunc(fnHash);
        _tconf->set("partitioner_cb", _ex_partition_cb, errstr);
    }

    LOG_INFO("KafkaOpPdu InitConf Success host:%s | topic: %s"
        , brokers.c_str(), _topicname.c_str());

    return ReInitProducer();
}

bool KafkaOpPdu::ReInitProducer()
{
    if (_bInitProducer) return true;

    if (_topicproducer != nullptr)
    {
        delete _topicproducer;
        _topicproducer = nullptr;
    }

    if (_producer != nullptr)
    {
        _producer->poll(10);
        delete _producer;
        _producer = nullptr;
    }

    std::string errstr;
    _producer = RdKafka::Producer::create(_conf, errstr);
    if (!_producer)
    {
        LOG_ERROR("KafkaOpPdu Failed to create producer: %s", errstr.c_str());
        return false;
    }

    LOG_INFO("KafkaOpPdu Created producer: %s", _producer->name().c_str());

    _topicproducer = RdKafka::Topic::create(_producer, _topicname,
        _tconf, errstr);
    if (!_topicproducer)
    {
        LOG_ERROR("KafkaOpPdu Failed to create topicproducer: %s", errstr.c_str());
        return false;
    }

    LOG_INFO("KafkaOpPdu Created producer: %s, topic: %s"
        , _producer->name().c_str(), _topicname.c_str());
    _bInitProducer = true;
    return true;
}

bool KafkaOpPdu::ProducerGroup(const char* szmsg, const uint32_t len, const std::string& key)
{
    return Producer(RdKafka::Topic::PARTITION_UA, szmsg, len, key);
}

void KafkaOpPdu::OnClose(const std::string& errstr)
{
    //    _bInitProducer = false;
    KafkaOpBase::OnClose(errstr);
}

bool KafkaOpPdu::Producer(int32_t partition, const char* szmsg, const uint32_t len)
{
    return this->Producer(partition, szmsg, len, "");
}

bool KafkaOpPdu::Producer(int32_t partition, const char* szmsg, const uint32_t len, const std::string& key)
{
    if (!ReInitProducer()) return false;
    RdKafka::ErrorCode resp;

    if (!key.empty())
    {
        resp = _producer->produce(_topicproducer, partition,
            RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
            (void*) szmsg, len, key.c_str(), key.length(), nullptr);
    }
    else
    {
        resp = _producer->produce(_topicproducer, partition,
            RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
            (void*) szmsg, len, nullptr, 0, nullptr);
    }

    if (resp != RdKafka::ERR_NO_ERROR)
    {
        LOG_ERROR("KafkaOpPdu Produce failed: %s", RdKafka::err2str(resp).c_str());
        return false;
    }
    _producer->poll(0);
    return true;
}