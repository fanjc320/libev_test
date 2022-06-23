/*
 *
 *      Author: venture
 */

#include "pch.h"
#include <typeinfo>

#include "pb_parse.h"
#include "../log/LogMgr.h"
#include "../platform/func_stack.h"
#include <google/protobuf/message.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/logging.h>

using namespace std;

void _LogHandler(pb::LogLevel level, const char* filename, int line,
    const std::string& message)
{
    switch (level)
    {
        case google::protobuf::LOGLEVEL_INFO:
            break;
        case google::protobuf::LOGLEVEL_WARNING:
            LOG_WARNING("pb file: %s:%d, %s", filename, line, message.c_str());
            break;
        case google::protobuf::LOGLEVEL_ERROR:
            LOG_ERROR("pb file: %s:%d, %s", filename, line, message.c_str());
            break;
        case google::protobuf::LOGLEVEL_FATAL:
            LOG_ERROR("pb file: %s:%d, %s", filename, line, message.c_str());
            break;
        default:
            LOG_ERROR("pb file: %s:%d, %s", filename, line, message.c_str());
            break;
    }
}

void set_pb_log_handle()
{
    pb::SetLogHandler(_LogHandler);
}

bool pb2_array(const pb::MessageLite* pMsg, void* pStr, size_t len)
{
    if (pMsg == nullptr || pStr == nullptr)
        return false;

    try
    {
        if (!pMsg->SerializeToArray(pStr, (int)len))
        {
            string strStack;
            func_stack::stack(strStack);
            LOG_ERROR("%s-%s", typeid(*pMsg).name(), strStack.c_str());

            LOG_ERROR("ser %s proto , fields:%s!", typeid(*pMsg).name()
                , pMsg->InitializationErrorString().c_str());
            return false;
        }
        return true;
    }
    catch (const std::exception& e)
    {
        string strStack;
        func_stack::stack(strStack);
        LOG_ERROR("%s-%s", typeid(*pMsg).name(), strStack.c_str());

        LOG_ERROR("ser %s proto error %s, fields:%s!", typeid(*pMsg).name()
            , e.what(), pMsg->InitializationErrorString().c_str());
    }
    return false;
}

bool pb2_array(const pb::MessageLite* pMsg, std::string* pStr)
{
    if (pMsg == nullptr || pStr == nullptr)
        return false;

    try
    {
        if (!pMsg->SerializeToString(pStr))
        {
            string strStack;
            func_stack::stack(strStack);
            LOG_ERROR("%s-%s", typeid(*pMsg).name(), strStack.c_str());

            LOG_ERROR("ser %s proto , fields:%s!", typeid(*pMsg).name()
                , pMsg->InitializationErrorString().c_str());
            return false;
        }
        return true;
    }
    catch (const std::exception& e)
    {
        string strStack;
        func_stack::stack(strStack);
        LOG_ERROR("%s-%s", typeid(*pMsg).name(), strStack.c_str());

        LOG_ERROR("ser %s proto error %s, fields:%s!", typeid(*pMsg).name()
            , e.what(), pMsg->InitializationErrorString().c_str());
    }
    return false;
}

bool pb2_array(const pb::MessageLite& oMsg, std::string& oStr)
{
    return pb2_array(&oMsg, &oStr);
}

bool array2_pb(pb::MessageLite* pMsg, const void* pData, size_t nSize)
{
    if (nullptr == (pData) || pMsg == nullptr)
        return false;

    try
    {
        if (!pMsg->ParseFromArray(pData, (int)nSize))
        {
            string strStack;
            func_stack::stack(strStack);
            LOG_ERROR("%s-%s", typeid(*pMsg).name(), strStack.c_str());

            LOG_ERROR("parse %s proto error, fields:%s!", typeid(*pMsg).name(), pMsg->InitializationErrorString().c_str());
            return false;
        }
    }
    catch (const std::exception& e)
    {
        string strStack;
        func_stack::stack(strStack);
        LOG_ERROR("%s-%s", typeid(*pMsg).name(), strStack.c_str());

        LOG_ERROR("parse %s proto error %s, fields:%s!", typeid(*pMsg).name(), e.what(), pMsg->InitializationErrorString().c_str());
        return false;
    }
    return true;
}