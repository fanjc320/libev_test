/*
 *
 *      Author: venture
 */
#pragma once

#include <fstream>
#include <stdio.h>
#include "dtype.h"
#include <string>

#include "server/AppConfigBase.h"
#include <stl/map_list.h>
#include "server/SvrModuleBase.h"

class LogicThread;
class EventTimer;
class SvrArgOpt;
class ArgOptBase;

extern SvrAppBase* GetMainApp();
#define APP_REG_MODULE(T, HAS_FRAME, HAS_TICK) GetMainApp()->RegMoudle((T), (HAS_FRAME), (HAS_TICK))

class SvrAppBase
{
protected:
    SvrAppBase(const std::string& strAppName = "");
    virtual ~SvrAppBase();

public:
    void   SetAutoOpt(bool val) { _bAutoOpt = val; }
    void   SetAutoConfig(bool val) { _bAutoConfig = val; }

    static uint32_t GetAreanNo();
    static uint32_t GetIndex();

    static ArgOptBase* GetArgBase();
    static void SetArgBase(ArgOptBase* pArg);

    static void SetConfigBase(AppConfigBase* pConfig);
    static AppConfigBase* GetConfigBase();

    template<typename T>
    static void SetConfigBase()
    {
        SetConfigBase(new T());
    }

    template<typename T>
    static void SetArg()
    {
        SetArgBase(new T());
    }

    template<typename T>
    static T* GetArg()
    {
        return dynamic_cast<T*>(GetArgBase());
    }

    static const std::string&  GetAppName();
    static void SetAppName(const std::string& strAppName);
    static std::string GetAppKey(char chSplit = '.');

    virtual bool Arg(int argc, char* argv[]);
    virtual bool MainLoop();

public:
    virtual bool RegModule() { return true; }
    virtual int32_t Init(uint64_t millFrame = 20);

public:
    std::string Version() { return _strVersion; }

    LogicThread*     GetLogicThread();

    std::string ReadVersion();
    std::string GetVersion();

    void ReloadVersion();
    bool IsNewVersion();

    bool RegMoudle(SvrModuleBase* pMoudle, bool HasFrame, bool HasTick);
    SvrModuleBase* GetMoudle(const std::string& strName);

protected:
    virtual void Stop();
    virtual void Fini();
    virtual bool Reload();
    virtual bool Run();

protected:
    virtual std::string _MakePidName();
    void _WritePidFile();

    virtual int32_t  _Init();
    virtual bool _Run();
    virtual void _Fini();

    virtual void _OnSignalReload(int32_t signal);
    virtual void _OnSignalStop(int32_t signal);

    virtual void _OnStop();
    virtual void _OnFrame(uint64_t qwTm);
    virtual void _OnTick(uint64_t qwTm);

    //  ���ݳ�ʼ��
    virtual bool _DBInit();
    virtual void _DBFini();

    // ��Դ��ʼ��
    virtual bool _ResInit();
    // ��Դ����
    virtual bool _ResAssembler();
    virtual void _ResFini();

    // �߼���ʼ��: �����Ҫ������ʼ��Э��ע��
    // timer�ص�, �ȼ������ݺ���Դ, ��׼���߼�
    virtual bool _LogicInit();
    virtual void _LogicFini();

    virtual bool _PreReload();

    // ��Դ��reload����
    virtual bool _Reload();
    // ��reload֮����Ҫ��һЩ���֪ͨ��ϵͳ֪ͨ
    virtual bool _ReloadNotice();
    // reload֮ǰ�Ĵ������������, ����, �����

    bool IsResume();
    void SetResume(bool val);

protected:
    LogicThread*    _pLogicThread = nullptr;
    EventTimer*     _pEventFrame = nullptr;
    EventTimer*     _pEventTick = nullptr;
    bool            _bAutoOpt = true;
    bool            _bAutoConfig = true;

    uint64_t        _qwPreMill = 0;
    uint64_t        _qwPreTick = 0;

    std::string     _strPidPath;
    std::string     _strLocalIp;
    std::string     _strVersion;
    std::string     _strVersionNew;
    map_list<std::string, SvrModuleBase*> _mapModule;
    map_list<std::string, SvrModuleBase*> _mapModuleFrame;
    map_list<std::string, SvrModuleBase*> _mapModuleTick;
};