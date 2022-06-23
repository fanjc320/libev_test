/*
 *
 *      Author: venture
 */
#include "pch.h"
#ifdef _MSC_VER
#include <iostream>
#include <sys/timeb.h>
#include <time.h>
#include <windows.h>
#include "const.h"

#else
#include <stdio.h>
#include "dirent.h"
#endif
#include <iostream>

#include "time/TimeCount.h"
#include "time/TimeDateX.h"
#include "time/time_func_x.h"
#include "platform/file_func.h"
#include "memory/malloc_x.h"
#include "LogMgr.h"
#include "print_color.h"
#include "const.h"

const char* szLogLevel[(int32_t)ELogLevel::End + 1] = {"none", "debug", "info", "warning", "error", "custom" };
size_t g_nSubBegin = -1;

LogMgr   g_logMgr;
LogMgr::LogMgr()
{
	//set default log level.
	_arrLogLevel[(int32_t)ELogTarget::Console] = ELogLevel::Info;
	_arrLogLevel[(int32_t)ELogTarget::File] = ELogLevel::Info;
	//setlocale(LC_ALL, "chs");

	_nIndex = 0;

	_nLock = 0;
	_nQueueIndex = 0;
	_bExit = false;
}

LogMgr::~LogMgr()
{
	UnInit();
}

LogMgr& LogMgr::GetMgr()
{
	return g_logMgr;
}

LogMgr::sLog* LogMgr::NewLogItem()
{
    static size_t nMalloc = sizeof(LogMgr::sLog);
    uint8_t* p = (uint8_t*)msg_alloc(nMalloc);
	if (p == nullptr) return nullptr;
	sLog* pData = new (p) sLog();
	return pData;
}

void LogMgr::FreeLogItem(LogMgr::sLog* pLog)
{
    if (pLog == nullptr) return;
	MSG_FREE(pLog);
}

bool LogMgr::Init(size_t area, size_t index, bool one_file)
{
	if (_bInit) return true;

	_bInit = true;
	_nIndex = index;
	_nArea = area;
	_bOneFile = one_file;
	_thread = std::thread(&LogMgr::Update, this);
	return true;
}

void LogMgr::UnInit()
{
	if (_bExit) return;
	_bExit = true;
	if (_thread.joinable())
		_thread.join();

	_Print_log(_nQueueIndex);
	_nQueueIndex ^= 1;
	_Print_log(_nQueueIndex);

	for (auto itr = _mapfile.begin(); itr != _mapfile.end(); ++itr)
	{
		if (itr->second._file != nullptr)
			fclose(itr->second._file);
	}
}

unsigned SP_THREAD_CALL LogMgr::Update(void* pThis)
{
	LogMgr* pLogMgr = (LogMgr*)pThis;
	if (!pLogMgr) return 0;
	while (!pLogMgr->_bExit)
	{
		pLogMgr->PopLog();
	}
	return 0;
}

inline uint64_t SyncCompareExchange(volatile uint64_t* pDest, uint64_t uOldVal, uint64_t uNewVal)
{
#ifdef _MSC_VER
	return _InterlockedCompareExchange64((volatile __int64*)pDest, (__int64)uNewVal, (__int64)uOldVal);
#else
	return __sync_val_compare_and_swap(pDest, uOldVal, uNewVal);
#endif
}

void LogMgr::PopLog()
{
	int nTempIndex = _nQueueIndex;
	while (1 == SyncCompareExchange(&_nLock, 0, 1));
	_nQueueIndex ^= 1;
	_nLock = 0;

	try
	{
		if (_queueLogs[nTempIndex].empty())
		{
			_CheckLogFileDate();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			return;
		}
		else
		{
			_Print_log(nTempIndex);
		}
	}
	catch (const std::exception& e)
	{
		printf("LogMgr::PopLog error %s\n", e.what());
	}
}

bool LogMgr::PushLog(sLog* pLog)
{
	bool bRet = false;
	while (1 == SyncCompareExchange(&_nLock, 0, 1));
	if (pLog->eLogType == ELogLevel::Error || _queueLogs[_nQueueIndex].size() < 10000)
	{
		_queueLogs[_nQueueIndex].push(pLog);
		bRet = true;
	}

	_nLock = 0;
	return bRet;
}

void LogMgr::SetTargetLevel(ELogTarget eTar, ELogLevel eLev)
{
	_arrLogLevel[(size_t)eTar] = eLev;
}

template <typename T>
void PrintToConsle(ELogLevel eLogType, T clr, const char* szLog, const char* szLogHead)
{
	print_color(clr, "%s %s \r\n", szLogHead, szLog);
}

void LogMgr::_Print_log(int nIndex)
{
	if (nIndex > 1 || nIndex < 0)
		return;

	while (!_queueLogs[nIndex].empty())
	{
		sLog* pLog = _queueLogs[nIndex].front();
		_queueLogs[nIndex].pop();

		if (pLog == nullptr) continue;

		if (pLog->eLogType >= _arrLogLevel[(size_t)ELogTarget::Console])
		{
			switch (pLog->eLogType)
			{
			case ELogLevel::Error:
				PrintToConsle(pLog->eLogType, CLR_RED, pLog->szBuff, pLog->szBuffHead);
				break;
			case ELogLevel::Warning:
				PrintToConsle(pLog->eLogType, CLR_YELLOW, pLog->szBuff, pLog->szBuffHead);
				break;
			case ELogLevel::Custom:
				PrintToConsle(pLog->eLogType, CLR_GREEN, pLog->szBuff, pLog->szBuffHead);
				break;
			case ELogLevel::Debug:
                PrintToConsle(pLog->eLogType, CLR_BLUE, pLog->szBuff, pLog->szBuffHead);
                break;
			default:
				PrintToConsle(pLog->eLogType, CLR_RESET, pLog->szBuff, pLog->szBuffHead);
				break;
			}
		}

		if (pLog->eLogType >= _arrLogLevel[(size_t)ELogTarget::File])
		{
			bool bReset = false;
			for (int i = 0; i < 2; ++i)
			{
				FILE* psFile = _GetLogFile(pLog, bReset);
				if (psFile)
				{
					int nRes = fprintf(psFile, "%s %s\r\n", pLog->szBuffHead, pLog->szBuff);
					if (nRes < 0)
					{
						bReset = true;
						continue;
					}
					nRes = fflush(psFile);
					if (nRes == EOF)
					{
						bReset = true;
						continue;
					}
				}
				break;
			}
		}
		FreeLogItem(pLog);
	}
}

std::string LogMgr::_MakeFileName(const char* szLogName)
{
	std::string strFileName = file::get_exec_name();
	std::string strDate = TimeDateX::Str_s(ETimeStrType::DATE_SIMPLE);

	std::string ssName;
    ssName = strFileName + "_" + strDate + "_" + std::to_string(_nArea) + "_"
          + std::to_string(_nIndex) + "_" + szLogName + ".log";

	std::string strPath;
	strPath = file::get_local_path(DEF_LOG_PATH) + strDate +"/";

	file::create_dir(strPath);
	std::string strFull = strPath + ssName;
	return strFull;
}

void LogMgr::_CheckLogFileDate(int64_t qwDate)
{
	if (qwDate == 0)
		qwDate = TimeDateX::Number_s(ETimeStrType::DATE_SIMPLE);

	if (_qwDatePre != qwDate || _tmReSet.On(get_curr_time()))
	{
		for (auto& itr : _mapfile)
		{
			if (itr.second._file != nullptr)
			{
				fclose(itr.second._file);
				itr.second._file = nullptr;
			}
		}

		_qwDatePre = qwDate;
	}
}

FILE* LogMgr::_GetLogFile(sLog* pLog, bool bReset)
{
	if (pLog == nullptr) return nullptr;

	int64_t nDate = TimeDateX::Number_s(ETimeStrType::DATE_SIMPLE);
	sLogFile* pLogFile = nullptr;

	pLogFile = &(_mapfile[pLog->szFileName]);
	if (pLogFile == nullptr) return nullptr;

	_CheckLogFileDate(nDate);

	if (pLogFile->_file != nullptr)
	{
		if (bReset)
		{
			fclose(pLogFile->_file);
			pLogFile->_file = nullptr;
		}
		else
			return pLogFile->_file;
	}

	std::string strfile;
	strfile = _MakeFileName(pLog->szFileName);

	FILE* psLogFile = nullptr;

#ifdef _MSC_VER
	psLogFile = fopen(strfile.c_str(), "ab+");
#else
	psLogFile = fopen(strfile.c_str(), "ab+e");
#endif
	if (pLogFile->_file != nullptr)
		fclose(pLogFile->_file);

	pLogFile->_date = nDate;
	pLogFile->_file = psLogFile;
	pLogFile->_filename = strfile;

	return psLogFile;
}

bool LogMgr::_BuildLog(ELogLevel eLL, const char* pLogName, const char* pFileName, const char* pFuncName, int nLine, sLog* pLog)
{
	if (eLL == ELogLevel::End || pLog == nullptr) return false;

	if (_arrLogLevel[(int32_t)ELogTarget::Console] > eLL && _arrLogLevel[(size_t)ELogTarget::File] > eLL)
		return false;

	if (eLL != ELogLevel::Custom) pLogName = szLogLevel[(int32_t)eLL];
	if (pLogName == nullptr) return false;

    const char* pFileNameNew = pFileName;
	if (g_nSubBegin == -1)
	{
		std::string strTemp = pFileName;
#ifdef _MSC_VER
        g_nSubBegin = strTemp.find("\\src\\");
#else
        g_nSubBegin = strTemp.find("/src/");
#endif // _MSC_VER
		if (g_nSubBegin == std::string::npos)
			g_nSubBegin = -1;
		else
			g_nSubBegin = g_nSubBegin + 1;
	}
	if (g_nSubBegin != -1) 
		pFileNameNew = pFileNameNew + g_nSubBegin;

	sprintf_safe(pLog->szBuffHead, "[%-7s] %s: %s:%s:%d", pLogName, TimeDateX::Str_s().c_str()
		, pFileNameNew, pFuncName, nLine);

	pLog->eLogType = eLL;
	if(_bOneFile)
		strncpy_safe(pLog->szFileName, szLogLevel[(uint32_t)ELogLevel::Info]);
	else
		strncpy_safe(pLog->szFileName, pLogName, strlen(pLogName));

	return PushLog(pLog);
}
