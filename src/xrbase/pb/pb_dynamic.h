/*
 *
 *      Author: venture
 */
#pragma once

#include <string>

namespace google
{
    namespace protobuf
    {
        class Message;
    };
};

namespace pb_dynamic
{
    pb::Message* create_msg(const std::string& sProtoName);
    pb::Message* parse_from_string(const std::string& sProtoName, const std::string& sProtoData);
    pb::Message* parse_from_array(const std::string& sProtoName, const char* szProtoData, const uint32_t iLen);
    void delete_msg(pb::Message*& pMsg);
};