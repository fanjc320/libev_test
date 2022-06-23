// Demo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "pch.h"
#include <iostream>
#include <stdint.h>
#include "dtype.h"
#include "XrLogApp.h"
#include "log/LogMgr.h"

int main(int argc, char* argv[])
{
    SvrAppBase* pApp = XrLogApp::GetInstance();
    bool ret = pApp->Arg(argc, argv);
    ERROR_LOG_EXIT0(ret);
    ret = pApp->MainLoop();
    if (!ret)
    {
        printf("loop Error!!!");
    }
    return 1;
Exit0:
    return 0;
}
