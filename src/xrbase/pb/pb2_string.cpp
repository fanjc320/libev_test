/*
 *
 *      Author: venture
 */
#include "pch.h"
#include <string>
#include <stdint.h>

#include "pb2_string.h"
#include "google/protobuf/message.h"
#include "log/LogMgr.h"

#include "google/protobuf/util/json_util.h"

void log_append(std::string &out_str, const std::string &item, const std::string filter);
int pb2_repeated_string(const pb::Message &pb_msg, const pb::FieldDescriptor *field_des,
                          uint32_t field_index, std::string &out_str, const std::string filter);
int pb2_field_string(const pb::Message &pb_msg, const pb::FieldDescriptor *field_des,
                       std::string &out_str, const std::string filter);

void log_append(std::string &out_str, const std::string &item, const std::string filter)
{
    if (!out_str.empty())
        out_str.append(filter);

    out_str.append(item);
}

int pb2_json(const pb::Message &pb_msg, std::string &out_str)
{
    pb::util::JsonPrintOptions opts;
    opts.always_print_primitive_fields = true;
    opts.always_print_enums_as_ints = true;
    opts.preserve_proto_field_names = true;
    pb::util::Status sts = pb::util::MessageToJsonString(pb_msg, &out_str, opts);
    CHECK_LOG_ERROR(sts.ok());
    return 0;
}

int pb2_string(const pb::Message &pb_msg, std::string &out_str, const std::string filter)
{
    const pb::FieldDescriptor *field_des = nullptr;
    const pb::Descriptor *msg_desc       = pb_msg.GetDescriptor();
    const pb::Reflection *pb_ref         = pb_msg.GetReflection();

    for (int32_t i = 0; i < msg_desc->field_count(); ++i)
    {
        field_des = msg_desc->field(i);
        if (field_des->is_repeated())
        {
            for (int32_t i = 0; i < pb_ref->FieldSize(pb_msg, field_des); i++)
            {
                pb2_repeated_string(pb_msg, field_des, i, out_str, filter);
            }
        }
        else
        {
            pb2_field_string(pb_msg, field_des, out_str, filter);
        }
    }
    return 0;
}

int pb2_repeated_string(const pb::Message &pb_msg, const pb::FieldDescriptor *field_des,
                          uint32_t field_index, std::string &out_str, const std::string filter)
{
    const pb::Reflection *pb_ref = pb_msg.GetReflection();

    switch (field_des->cpp_type())
    {
    case pb::FieldDescriptor::CPPTYPE_INT32: // TYPE_INT32, TYPE_SINT32, TYPE_SFIXED32
        log_append(out_str, std::to_string(pb_ref->GetRepeatedInt32(pb_msg, field_des, field_index)), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_INT64: // TYPE_INT64, TYPE_SINT64, TYPE_SFIXED64
        log_append(out_str, std::to_string((long long)pb_ref->GetRepeatedInt64(pb_msg, field_des, field_index)), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_UINT32: // TYPE_UINT32, TYPE_FIXED32
        log_append(out_str, std::to_string(pb_ref->GetRepeatedUInt32(pb_msg, field_des, field_index)), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_UINT64: // TYPE_UINT64, TYPE_FIXED64
        log_append(out_str,
            std::to_string((unsigned long long)pb_ref->GetRepeatedUInt64(pb_msg, field_des, field_index)), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_DOUBLE: // TYPE_DOUBLE
        log_append(out_str, std::to_string((long long)pb_ref->GetRepeatedDouble(pb_msg, field_des, field_index)), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_FLOAT: // TYPE_FLOAT
        log_append(out_str, std::to_string((long long)pb_ref->GetRepeatedFloat(pb_msg, field_des, field_index)), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_BOOL: // TYPE_BOOL
        log_append(out_str, std::to_string(pb_ref->GetRepeatedBool(pb_msg, field_des, field_index)), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_ENUM: // TYPE_ENUM
        log_append(out_str, std::to_string(pb_ref->GetRepeatedEnumValue(pb_msg, field_des, field_index)), filter);
        break;
        break;
    case pb::FieldDescriptor::CPPTYPE_STRING: // TYPE_STRING, TYPE_BYTES
        log_append(out_str, pb_ref->GetRepeatedString(pb_msg, field_des, field_index), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_MESSAGE: // TYPE_MESSAGE, TYPE_GROUP
        pb2_string(pb_ref->GetRepeatedMessage(pb_msg, field_des, field_index), out_str, filter);
        break;
    }

    return 0;
}

int pb2_field_string(const pb::Message &pb_msg, const pb::FieldDescriptor *field_des,
                       std::string &out_str, const std::string filter)
{
    const pb::Reflection *pb_ref = pb_msg.GetReflection();

    switch (field_des->cpp_type())
    {
    case pb::FieldDescriptor::CPPTYPE_INT32: // TYPE_INT32, TYPE_SINT32, TYPE_SFIXED32
        log_append(out_str, std::to_string(pb_ref->GetInt32(pb_msg, field_des)), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_INT64: // TYPE_INT64, TYPE_SINT64, TYPE_SFIXED64
        log_append(out_str, std::to_string((long long)pb_ref->GetInt64(pb_msg, field_des)), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_UINT32: // TYPE_UINT32, TYPE_FIXED32
        log_append(out_str, std::to_string(pb_ref->GetUInt32(pb_msg, field_des)), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_UINT64: // TYPE_UINT64, TYPE_FIXED64
        log_append(out_str, std::to_string((unsigned long long)pb_ref->GetUInt64(pb_msg, field_des)), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_DOUBLE: // TYPE_DOUBLE
        log_append(out_str, std::to_string((long long)pb_ref->GetDouble(pb_msg, field_des)), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_FLOAT: // TYPE_FLOAT
        log_append(out_str, std::to_string((long long)pb_ref->GetFloat(pb_msg, field_des)), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_BOOL: // TYPE_BOOL
        log_append(out_str, std::to_string(pb_ref->GetBool(pb_msg, field_des)), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_ENUM: // TYPE_ENUM
        log_append(out_str, std::to_string(pb_ref->GetEnumValue(pb_msg, field_des)), filter);
        break;
        break;
    case pb::FieldDescriptor::CPPTYPE_STRING: // TYPE_STRING, TYPE_BYTES
        log_append(out_str, pb_ref->GetString(pb_msg, field_des), filter);
        break;
    case pb::FieldDescriptor::CPPTYPE_MESSAGE: // TYPE_MESSAGE, TYPE_GROUP
        pb2_string(pb_ref->GetMessage(pb_msg, field_des), out_str, filter);
        break;
    }

    return 0;
}