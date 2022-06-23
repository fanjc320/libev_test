#pragma once
#include <string>
#include <vector>
#include "net/base/MsgQueue.h"

#include "dtype.h"
#include "rdkafkacpp.h"

using namespace std;
class OpRebalanceCb : public RdKafka::RebalanceCb
{
private:
    static void part_list_print(const std::vector<RdKafka::TopicPartition*>& partitions)
    {
        std::string ss;
        for (unsigned int i = 0; i < partitions.size(); i++)
        {
            ss = ss + partitions[i]->topic() +
                "[" + std::to_string(partitions[i]->partition()) + "], ";
        }
        LOG_INFO("KAFKA Rebalance : %s", ss.c_str());
    }

public:
    void rebalance_cb(RdKafka::KafkaConsumer* consumer,
        RdKafka::ErrorCode err,
        std::vector<RdKafka::TopicPartition*>& partitions) override
    {
        LOG_INFO("KAFKA RebalanceCb: %s", RdKafka::err2str(err).c_str());
        part_list_print(partitions);

        if (err == RdKafka::ERR__ASSIGN_PARTITIONS)
        {
            consumer->assign(partitions);
        }
        else
        {
            consumer->unassign();
        }
    }
};

class OpEventCb : public RdKafka::EventCb
{
public:
    OpEventCb() : _fn_exit(nullptr) { }

    void InitFn(FUN_EVENT_ERROR fn) { _fn_exit = fn; }
    void event_cb(RdKafka::Event& event) override
    {
        switch (event.type())
        {
            case RdKafka::Event::EVENT_ERROR:
                {
                    std::string ss;
                    ss = ss + ", event=" + event.str();
                    ss = ss + ", errid=" + std::to_string(event.err());
                    ss = ss + ", severity=" + std::to_string(event.severity());
                    ss = ss + ", err=" + RdKafka::err2str(event.err());
                    std::string strError = ss;
                    strError = "KAFKA ERROR: " + strError;
                    LOG_ERROR("%s", strError.c_str());

                    //if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN)
                    //if(event.err() != RdKafka::ERR__TRANSPORT)
                    {
                        if (_fn_exit != nullptr)
                            _fn_exit(strError);
                    }
                }
                break;

            case RdKafka::Event::EVENT_STATS:
                LOG_ERROR("KAFKA \"STATS\": %s", event.str());
                break;

            case RdKafka::Event::EVENT_LOG:
                {
                    std::string ss;
                    ss = ss + ", severity=" + std::to_string(event.severity());
                    ss = ss + ", fac=" + event.fac();
                    ss = ss + ", event=" + event.str();
                    ss = ss + ", errid=" + std::to_string(event.err());
                    std::string strError = ss;
                    strError = "KAFKA LOG: " + strError;
                    LOG_ERROR("%s", strError.c_str());
                    // 			if (event.severity() == RdKafka::Event::EVENT_SEVERITY_ERROR)
                    if (_fn_exit != nullptr)
                        _fn_exit(strError);
                }
                break;

            default:
                LOG_ERROR("KAFKA EVENT %i (%s): %s", event.type()
                    , RdKafka::err2str(event.err()).c_str(), event.str().c_str());
                break;
        }
    }

private:
    FUN_EVENT_ERROR _fn_exit = nullptr;
};

class OpHashPartitionerCb : public RdKafka::PartitionerCb
{
public:
    int32_t partitioner_cb(const RdKafka::Topic* topic, const std::string* key,
        int32_t partition_cnt, void* msg_opaque) override
    {
        if (_fn_hash != nullptr)
            return _fn_hash(key->c_str(), key->size()) % partition_cnt;
        else
            return djb_hash(key->c_str(), key->size()) % partition_cnt;
    }

    void InitFunc(FUN_HASH_PARTION fn) { _fn_hash = fn; }
private:

    FUN_HASH_PARTION _fn_hash = nullptr;

    static inline int32_t djb_hash(const char* str, size_t len)
    {
        int32_t hash = 5381;
        for (size_t i = 0; i < len; i++)
            hash = ((hash << 5) + hash) + str[i];
        return hash;
    }
};

class OpConsumeCb : public RdKafka::ConsumeCb
{
public:

    void InitFunc(FUN_MSG_RECV fn)
    {
        _fn_msg_func = fn;
    }

    void consume_cb(RdKafka::Message& msg, void* opaque) override
    {
        switch (msg.err())
        {
            case RdKafka::ERR_NO_ERROR:
                if (_fn_msg_func != nullptr)
                    _fn_msg_func(static_cast<const char*>(msg.payload()), msg.len());
                break;
            default:
                break;
        }
    }

private:
    FUN_MSG_RECV _fn_msg_func = nullptr;
};
