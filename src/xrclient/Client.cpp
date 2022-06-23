// Demo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "pch.h"
#include <iostream>
#include <stdint.h>
#include "dtype.h"
#include "ClientApp.h"

int main(int argc, char* argv[])
{
    SvrAppBase* pApp = ClientApp::GetInstance();
    pApp->Arg(argc, argv);
    pApp->MainLoop();
}
