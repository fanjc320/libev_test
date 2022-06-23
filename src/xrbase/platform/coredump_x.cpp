/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "log/LogMgr.h"

#include <signal.h>
#ifdef _MSC_VER
#include <windows.h>
#include <dbghelp.h> 
#include <stdio.h>
#pragma comment(lib, "dbghelp.lib")
#else
#include <sys/resource.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <syslog.h>
#endif // _MSC_VER

#include "coredump_x.h"
#include "time/TimeDateX.h"
#include "platform/func_stack.h"
#include "platform/file_func.h"
#include "SignalMgr.h"
#include "math/math_basic.h"
#include "platform/func_proc.h"

const uint32_t CALLSTACK_BUF_SIZE = 1024 * 4;
void write_call_list();

bool set_console_info(FUNC_SIGNAL_HANDLE fnReload, FUNC_SIGNAL_HANDLE fnExit)
{
#ifdef _MSC_VER
    HANDLE      hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD       BufferSize = { 108, 320 };
    SMALL_RECT  WinRect = { 0, 0, BufferSize.X - 1, 27 };

    SetConsoleScreenBufferSize(hConsole, BufferSize);
    SetConsoleWindowInfo(hConsole, true, &WinRect);

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    //SetConsoleCtrlHandler(&ConsoleHandlerRoutine, TRUE);
    g_SignalMgr.RegSigHandler(SIGBREAK, fnExit);

#else
    bool    bResult = false;
    int     nRetCode = 0;
    int     nNullFile = -1;
    rlimit  ResLimit;

    ResLimit.rlim_cur = UINT16_MAX;
    ResLimit.rlim_max = UINT16_MAX;

    setrlimit(RLIMIT_NOFILE, &ResLimit);
    CHECK_LOG_ERROR(nRetCode == 0);

    nRetCode = getrlimit(RLIMIT_CORE, &ResLimit);
    CHECK_LOG_ERROR(nRetCode == 0);

    ResLimit.rlim_cur = ResLimit.rlim_max;
    nRetCode = setrlimit(RLIMIT_CORE, &ResLimit);
    CHECK_LOG_ERROR(nRetCode == 0);

    g_SignalMgr.RegSigHandler(SIGUSR1, fnReload);
    g_SignalMgr.RegSigHandler(SIGUSR2, fnExit);
    g_SignalMgr.RegSigHandler(SIGSTOP, fnExit);

#endif
    g_SignalMgr.RegSigHandler(SIGINT, fnExit);
    g_SignalMgr.RegSigHandler(SIGTERM, fnExit);

    return true;
}

#ifdef _MSC_VER
LONG WINAPI CoreDump_Handler_Ex(struct _EXCEPTION_POINTERS* pExceptionPointers)
{
    write_call_list();

    SetErrorMode(SEM_NOGPFAULTERRORBOX);
    char szDumpFileName[1024] = { 0 };
    sprintf_s(szDumpFileName, sizeof(szDumpFileName), "%s\\%s.%s.%u.dmp",
        file::get_exec_path().c_str(),
        file::get_exec_name().c_str(),
        TimeDateX::Str_s(ETimeStrType::ALL_SIMPLE, std::time(nullptr)).c_str(),
        GetCurrentProcessId());

    HANDLE hDumpFile = CreateFile(szDumpFileName,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        0,
        CREATE_ALWAYS,
        0,
        0);

    if (hDumpFile != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION ExpParam;
        ExpParam.ThreadId = GetCurrentThreadId();
        ExpParam.ExceptionPointers = pExceptionPointers;
        ExpParam.ClientPointers = TRUE;

        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile,
            MiniDumpNormal,
            &ExpParam,
            nullptr,
            nullptr);

        CloseHandle(hDumpFile);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void CoreDump_Handler(int32_t signal)
{
    write_call_list();
    abort();
}

void write_call_list()
{
    char szDumpFileName[1024] = { 0 };
    std::string strText;
    func_stack::stack(strText);

    FILE* psLogFile = nullptr;
    sprintf_s(szDumpFileName, sizeof(szDumpFileName), "%s/%s.%s.%u.log",
        file::get_exec_path().c_str(),
        file::get_exec_name().c_str(),
        TimeDateX::Str_s(ETimeStrType::ALL_SIMPLE, std::time(nullptr)).c_str(), get_pid());
#ifdef _MSC_VER
    psLogFile = fopen(szDumpFileName, "ab+");
#else
    psLogFile = fopen(szDumpFileName, "ab+e");
#endif
    if (psLogFile != nullptr)
    {
        try
        {
            fprintf(psLogFile, "%s", strText.c_str());
        }
        catch (const std::exception&)
        {

        }
        fflush(psLogFile);
        fclose(psLogFile);
    }
}

void init_core_dumper()
{
    g_SignalMgr.RegSigHandler(SIGSEGV, CoreDump_Handler);
    g_SignalMgr.RegSigHandler(SIGFPE, CoreDump_Handler);
    g_SignalMgr.RegSigHandler(SIGILL, CoreDump_Handler);
#ifdef _MSC_VER
    SetUnhandledExceptionFilter(CoreDump_Handler_Ex);
#endif
}
