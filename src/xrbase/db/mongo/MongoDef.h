/*
 *
 *      Author: venture
 */
#pragma once

#include <functional>
#include "bsoncxx/document/view.hpp"

typedef std::function<bool(const bsoncxx::document::view*)> FUNC_MONGO_RET;

#define FORDO_TRY_BEGIN \
for (int i = 0; i < 60; ++i)\
{\
    try\
    {

#define FORDO_TRY_END(excep) \
        break;\
    }\
    catch (const excep& err)\
    {\
        if(MongoDbField::IsReconCode(err.code().value()))\
        {\
            LOG_ERROR("reconnect ts:%d", i + 1); \
            if (i >= 59) throw err; \
            this_thread::sleep_for(chrono::milliseconds(1000)); \
         }\
         else\
        {\
            throw err;\
            break;\
        }\
        if(i < 60) continue;\
    }\
    catch (const std::system_error &err)\
    {\
        LOG_ERROR("error: %s", err.what());\
        if(i < 60) continue;\
    }\
    catch (const std::exception &err)\
    {\
        LOG_ERROR("error: %s", err.what());\
        if(i < 60) continue;\
    }\
}


#define BEGIN_TRY_MD try


#define ENG_TRY_MD(qer1, qer2)   \
    catch(const mongocxx::operation_exception& err)\
    {\
        AddQueryLog(bsoncxx::to_json(qer1));\
        AddQueryLog(bsoncxx::to_json(qer2));\
        MongoDbField::SetErr(this, &err); \
        LOG_ERROR("caught table:%s,%s: %s, code:%d, typename:%s, qer:%s"\
            , _tblName.c_str(), _secTblName.c_str(), err.what()\
            , err.code().value(), typeid(err).name(), GetQueryLog()); \
        DoError(__FUNCTION__, __LINE__); \
        return false; \
    }\
    catch (const std::system_error &err)\
    {\
        AddQueryLog(bsoncxx::to_json(qer1));\
        AddQueryLog(bsoncxx::to_json(qer2));\
        SetErr(err.what(), err.code().value()); \
        LOG_ERROR("caught table:%s,%s: %s, code:%d, typename:%s, qer:%s"\
            , _tblName.c_str(), _secTblName.c_str(), err.what()\
            , err.code().value(), typeid(err).name(), GetQueryLog()); \
        DoError(__FUNCTION__, __LINE__); \
        return false; \
	}\
    catch (const std::exception &err)\
    {\
        AddQueryLog(bsoncxx::to_json(qer1));\
        AddQueryLog(bsoncxx::to_json(qer2));\
        SetErr(err.what()); \
        LOG_ERROR("caught table:%s,%s: %s, typename:%s, qer:%s"\
            , _tblName.c_str(), _secTblName.c_str(), err.what()\
            , typeid(err).name(), GetQueryLog()); \
        DoError(__FUNCTION__, __LINE__); \
        return false; \
	}

