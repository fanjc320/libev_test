/*
 *
 *      Author: venture
 */
#pragma  once

#include <queue>
#include <stdint.h>
#include <string>
#include <thread>
#include <mutex>
#include "dtype.h"
#include "platform/spthread.h"
#include "string/str_format.h"
#include "time/TimeCount.h"

 //enum type for log level.
enum class ELogLevel : int8_t
{
	None,
    Debug, 
	Info, 
	Warning, 
	Error, 
	Custom, 
	End
};

enum class ELogTarget : int8_t
{
	None,
    Console, 
	File, 
	End
};


struct sLogFile
{
    FILE* _file = nullptr;
    int64_t   _date = 0;
    std::string  _filename;
};

class LogMgr
{
    static const size_t LOG_BUF_SIZE = 2048;
    static const size_t LOG_FILENAME_SIZE = 256;
    static const size_t LOG_FILEPATH_HEAD = 256;

public:
	static LogMgr& GetMgr();

    struct sLog
    {
        ELogLevel eLogType = ELogLevel::Debug;
        char szFileName[LOG_FILENAME_SIZE] = {};
        char szBuff[LOG_BUF_SIZE] = {};
        char szBuffHead[LOG_FILEPATH_HEAD] = {};
    };

    static LogMgr::sLog* NewLogItem();
    static void FreeLogItem(LogMgr::sLog* pLog);

    LogMgr();
    virtual ~LogMgr();

    bool Init(size_t area, size_t index, bool one_file = false);
    void UnInit();
    static unsigned SP_THREAD_CALL Update(void* pThis);
    void PopLog();
    bool PushLog(sLog* pLog);

public:
    virtual void SetTargetLevel(ELogTarget eTar, ELogLevel eLev);

    template <typename... Args>
    bool Write(ELogLevel eLL, const char* szLogName, const char* szFile
        , const char* szFunc, int nLine, const char* szMsg, Args&&... args);
private:
    void _Print_log(int nIndex);
    std::string _MakeFileName(const char* szLogName);
    void _CheckLogFileDate(int64_t qwDate = 0);
    FILE* _GetLogFile(sLog* pLog, bool bReset = false);
    bool _BuildLog(ELogLevel eLL, const char* pLogName, const char* pFileName, const char* pFuncName, int nLine, sLog* pLog);

    ELogLevel _arrLogLevel[(size_t) (ELogTarget::End)];
    umap<std::string, sLogFile> _mapfile;

    bool	_bExit = false;
    size_t	_nIndex = 0;
    size_t	_nArea = 0;
    int64_t _qwDatePre = 0;
    TimeCount<600> _tmReSet;
    std::queue<sLog*> _queueLogs[2];

    volatile uint64_t _nLock;
    int	_nQueueIndex = 0;
    bool _bInit = false;
	bool _bOneFile = false;
    std::thread    _thread;
};

template <typename... Args>
bool LogMgr::Write(ELogLevel eLL, const char* pLogName, const char* pFileName
    , const char* pFuncName, int nLine, const char* pMsgFormat, Args&&... args)
{
    if (nullptr == pMsgFormat || nullptr == pFileName || nullptr == pFuncName)
        return false;

    LogMgr::sLog* pLog = LogMgr::NewLogItem();
    if (pLog == nullptr) return false;

    try
    {
        sprintf_safe(pLog->szBuff, pMsgFormat, std::forward<Args>(args)...);
        if (!_BuildLog(eLL, pLogName, pFileName, pFuncName, nLine, pLog))
        {
            LogMgr::FreeLogItem(pLog);
            return false;
        }
    }
    catch (const std::exception& e)
    {
        LogMgr::FreeLogItem(pLog);
		printf("LogMgr::PopLog error %s\n", e.what());
        return false;
    }

    return true;
}

#define LOG_MGR_INIT(x, y, z) LogMgr::GetMgr().Init((x), (y), (z))
#define LOG_SET_LEVEL(x, y) LogMgr::GetMgr().SetTargetLevel((x), (y))
#define LOG_MGR_UNINIT() LogMgr::GetMgr().UnInit()

#define LOG_DEBUG(fmt, ...) LogMgr::GetMgr().Write(ELogLevel::Debug, "", __FILE__, __FUNCTION__, __LINE__, (fmt), ##__VA_ARGS__);
#define LOG_INFO(fmt, ...) LogMgr::GetMgr().Write(ELogLevel::Info, "", __FILE__, __FUNCTION__, __LINE__, (fmt), ##__VA_ARGS__);
#define LOG_WARNING(fmt, ...) LogMgr::GetMgr().Write(ELogLevel::Warning, "", __FILE__, __FUNCTION__, __LINE__, (fmt), ##__VA_ARGS__);
#define LOG_ERROR(fmt, ...) LogMgr::GetMgr().Write(ELogLevel::Error, "", __FILE__, __FUNCTION__, __LINE__, (fmt), ##__VA_ARGS__);
#define LOG_CUSTOM(fname, fmt, ...) LogMgr::GetMgr().Write(ELogLevel::Custom, (fname), __FILE__, __FUNCTION__, __LINE__, (fmt), ##__VA_ARGS__);
//#define LOG_MINE(fname, fmt, ...) LOG_CUSTOM(fname, (std::string("thid:%d tick:%d time:%d")+std::string(fmt)).c_str(), GetCurrentThreadId(), GetTickCount64(), timeGetTime(),  ##__VA_ARGS__)
//#define LOG_MINE(fname, fmt, ...) LOG_CUSTOM(fname, (std::string("thid:%d tick:%d ")+std::string(fmt)).c_str(), GetCurrentThreadId(), GetTickCount64(),  ##__VA_ARGS__)
#define LOG_MINE(fname, fmt, ...) Sleep(200); LOG_CUSTOM(fname, (std::string("thid:%d tick:%d ")+std::string(fmt)).c_str(), GetCurrentThreadId(), GetTickCount64(),  ##__VA_ARGS__)
#define LOG_MY(fname, fmt, ...) std::string fm = (std::string("thid:%d tick:%d ") + std::string(fmt)); LOG_CUSTOM(fname, fm, GetCurrentThreadId(), GetTickCount64())
//LOG_CUSTOM("mine", " thid:%d time:%d tick:%d", GetCurrentThreadId(), GetCurrentTime(), GetTickCount64());

#define ERROR_EXIT0(condition) \
	do \
	{\
	if (!(condition))\
		{\
		goto Exit0;\
		}\
	} while (false);

#define ERROR_LOG_EXIT0(condition) \
	do \
	{\
	if (!(condition))\
		{\
		LOG_ERROR("Error! %s", #condition);\
		goto Exit0;\
		}\
	} while (false)

#define CHECK_LOG_ERROR(condition) \
	do \
	{\
	if (!(condition))\
		{\
		LOG_ERROR("Error! %s", #condition);\
		}\
	} while (false)

#define CHECK_LOG_RETVAL(condition, ret) \
	do \
	{\
	if (!(condition))\
		{\
		LOG_ERROR("Error! %s", #condition);\
		return (ret);\
		}\
	} while (false)

#define CHECK_LOG_RETNONE(condition) \
	do \
	{\
	if (!(condition))\
		{\
		LOG_ERROR("Error! %s", #condition);\
		return;\
		}\
	} while (false)

#define SUCCESS_EXIT1(condition) \
	do \
	{\
	if ((condition))\
		{\
		goto Exit1;\
		}\
	} while (false)

#define SUCCESS_LOG_EXIT1(condition) \
	do \
	{\
	if ((condition))\
		{\
		LOG_INFO("Success!");\
		goto Exit1;\
		}\
	} while (false)

#define  assert_break(condition) \
   	if (!(condition))\
		{\
		LOG_ERROR("Error! %s", #condition);\
		throw(#condition);\
		break;\
		}


#define  assert_continue(condition) \
   	if (!(condition))\
		{\
		LOG_ERROR("Error! %s", #condition);\
		throw(#condition);\
		continue;\
		}

#define  assert_retval(condition, ret) \
   	if (!(condition))\
		{\
		LOG_ERROR("Error! %s", #condition);\
		throw(#condition);\
		return (ret);\
		}

#define  assert_noeffect(condition) \
   	if (!(condition))\
		{\
		LOG_ERROR("Error! %s", #condition);\
		throw(#condition);\
		}

#define  assert_retnone(condition) \
   	if (!(condition))\
		{\
		LOG_ERROR("Error! %s", #condition);\
		throw(#condition);\
		return ;\
		}

