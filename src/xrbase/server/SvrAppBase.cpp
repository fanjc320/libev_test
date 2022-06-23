/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "SvrAppBase.h"
#include "time/time_func_x.h"
#include "net/LogicThread.h"
#include "platform/coredump_x.h"
#include "log/LogMgr.h"
#include "platform/func_proc.h"
#include "platform/file_func.h"
#include "string/str_format.h"
#include "server/SvrArgOpt.h"
#include "const.h"
#include "data/LoadConfigMgr.h"
#include "data/LoadConfigScript.h"
#include "module/ModuleData.h"
#include "platform/SignalMgr.h"

std::string     g_strAppName;
ArgOptBase*     g_pArgOpt = nullptr;
AppConfigBase*  g_pConfigBase = nullptr;
ModuleData      g_ModuleData;

SvrAppBase::SvrAppBase(const std::string& strAppName)
{
    g_strAppName = strAppName;
}

SvrAppBase::~SvrAppBase()
{

}

ArgOptBase* SvrAppBase::GetArgBase()
{
    return g_pArgOpt;
}

void SvrAppBase::SetArgBase(ArgOptBase* pArg)
{
    g_pArgOpt = pArg;
}

void SvrAppBase::SetConfigBase(AppConfigBase* pConfig)
{
    g_pConfigBase = pConfig;
}

AppConfigBase* SvrAppBase::GetConfigBase()
{
    return g_pConfigBase;
}

const std::string& SvrAppBase::GetAppName()
{
    if (g_strAppName.empty())
    {
        LOG_ERROR("AppName is null!!, maybe the appbase not exsit in this project!!!");
        throw("error!!!");
    }
    return g_strAppName;
}

void SvrAppBase::SetAppName(const std::string& strAppName)
{
    g_strAppName = strAppName;
}

std::string SvrAppBase::GetAppKey(char chSplit)
{
    return GetAppName() + chSplit + std::to_string(g_pArgOpt->GetAreanNo()) + chSplit + std::to_string(g_pArgOpt->GetIndex());
}

uint32_t SvrAppBase::GetAreanNo()
{
    if (g_pArgOpt == nullptr) return 0;
    return g_pArgOpt->GetAreanNo();
}

uint32_t SvrAppBase::GetIndex()
{
    if (g_pArgOpt == nullptr) return 0;
    return g_pArgOpt->GetIndex();
}

bool SvrAppBase::Arg(int argc, char* argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);

    bool ret = false;

    if (g_pArgOpt == nullptr && _bAutoOpt)
    {
        LOG_WARNING("default argopt !");
        g_pArgOpt = new SvrArgOpt();
    }

    if (g_pConfigBase == nullptr && _bAutoConfig)
    {
        LOG_WARNING("default config base !");
        g_pConfigBase = new AppConfigBase();
    }

    if (g_pArgOpt != nullptr)
    {
        ret = g_pArgOpt->Argv(argc, argv);
        ERROR_LOG_EXIT0(ret);
    }

    if (g_strAppName.empty())
    {
        if(g_pArgOpt != nullptr) g_strAppName = g_pArgOpt->GetAlias();
        if (g_strAppName.empty()) g_strAppName = file::get_exec_name();
    }

    if (g_pConfigBase != nullptr && g_pArgOpt != nullptr)
    {
        ret = g_pConfigBase->Load(g_pArgOpt->GetConfig());
        ERROR_LOG_EXIT0(ret);
    }

    if (_pLogicThread == nullptr)
    {
        _pLogicThread = LogicThread::New();
    }

    ERROR_LOG_EXIT0(_pLogicThread != nullptr);

    return true;
Exit0:
    return false;
}

bool SvrAppBase::MainLoop()
{
    if (_Init() != 1)
    {
        _OnStop();
        _Fini();
        return false;
    }

    _strLocalIp = get_local_ip();

    LOG_CUSTOM("start", "localip:%s", _strLocalIp.c_str());
    bool bRet = _Run();
    _Fini();
    return bRet;
}

LogicThread* SvrAppBase::GetLogicThread()
{
    return _pLogicThread;
}

int32_t SvrAppBase::Init(uint64_t millFrame)
{
    bool bRet = false;
    int ret = false;
    std::string strShm;
    std::string ini_name;

    if (_pEventFrame == nullptr)
    {
        _pEventFrame = new EventTimer(_pLogicThread->EvThread()->Base(), true, millFrame
            , std::bind(&SvrAppBase::_OnFrame, this, std::placeholders::_1));
        ERROR_LOG_EXIT0(_pEventFrame->Init());
    }

    if (_pEventTick == nullptr)
    {
        _pEventTick = new EventTimer(_pLogicThread->EvThread()->Base(), true, 1
            , std::bind(&SvrAppBase::_OnTick, this, std::placeholders::_1));
        ERROR_LOG_EXIT0(_pEventTick->Init());
    }

    bRet = RegModule();
    ERROR_LOG_EXIT0(bRet);

    ERROR_LOG_EXIT0(ret == 0);
    bRet = _DBInit();
    ERROR_LOG_EXIT0(bRet);
    bRet = _ResInit();
    ERROR_LOG_EXIT0(bRet);
    bRet = _ResAssembler();
    ERROR_LOG_EXIT0(bRet);
    bRet = _LogicInit();
    ERROR_LOG_EXIT0(bRet);

    return 1;
Exit0:
    LOG_ERROR("Init Error!");
    return 0;
}

bool SvrAppBase::Run()
{
    if (_pLogicThread != nullptr)
    {
        _pLogicThread->MainLoop();
        return true;
    }
    return false;
}

void SvrAppBase::Stop()
{
    if (_pLogicThread != nullptr)
        _pLogicThread->Stop();
    if (_pEventFrame)
        _pEventFrame->Stop();
    if (_pEventTick)
        _pEventTick->Stop();
}

void SvrAppBase::Fini()
{
    SAFE_DELETE(g_pArgOpt);
    SAFE_DELETE(g_pConfigBase);
    SAFE_DELETE(_pEventFrame);
    SAFE_DELETE(_pEventTick);
    if (_pLogicThread)
    {
        LogicThread::Delete(_pLogicThread);
        _pLogicThread = nullptr;
    }
}

bool SvrAppBase::Reload()
{
    bool bRet = false;
    ReloadVersion();
    bRet = IsNewVersion();
    if (!bRet) return true;

    bRet = _PreReload();
    ERROR_LOG_EXIT0(bRet);
    bRet = _Reload();
    ERROR_LOG_EXIT0(bRet);
    bRet = _ReloadNotice();
    ERROR_LOG_EXIT0(bRet);
    return true;
Exit0:
    return false;
}

std::string SvrAppBase::ReadVersion()
{
    std::string strRet;
    std::string filename = file::get_local_path("version.log");
    file::load_file(filename, strRet);
    strRet = trim_s(strRet);

    return strRet;
}

std::string SvrAppBase::GetVersion()
{
    return _strVersion;
}

void SvrAppBase::ReloadVersion()
{
    _strVersionNew = ReadVersion();
}

bool SvrAppBase::IsNewVersion()
{
    if (_strVersionNew.empty()) return false;
    return _strVersion != _strVersionNew;
}

bool SvrAppBase::RegMoudle(SvrModuleBase* pMoudle, bool HasFrame, bool HasTick)
{
    if (_mapModule.find(pMoudle->GetModuleID()) != nullptr)
    {
        LOG_ERROR("%s moudle is exsit!", pMoudle->GetModuleID().c_str());
        return false;
    }
    if (HasFrame)
    {
        if (_mapModuleFrame.find(pMoudle->GetModuleID()) != nullptr)
        {
            LOG_ERROR("%s frame moudle is exsit!", pMoudle->GetModuleID().c_str());
            return false;
        }
    }
    if (HasTick)
    {
        if (_mapModuleTick.find(pMoudle->GetModuleID()) != nullptr)
        {
            LOG_ERROR("%s tick moudle is exsit!", pMoudle->GetModuleID().c_str());
            return false;
        }
    }
    bool ret = _mapModule.insert(pMoudle->GetModuleID(), pMoudle);
    ERROR_LOG_EXIT0(ret);
    ret = _mapModuleFrame.insert(pMoudle->GetModuleID(), pMoudle);
    ERROR_LOG_EXIT0(ret);
    ret = _mapModuleTick.insert(pMoudle->GetModuleID(), pMoudle);
    ERROR_LOG_EXIT0(ret);
    return true;
Exit0:
    LOG_ERROR("%s moudle reg error!", pMoudle->GetModuleID().c_str());

    _mapModuleFrame.erase(pMoudle->GetModuleID());
    _mapModule.erase(pMoudle->GetModuleID());
    _mapModule.erase(pMoudle->GetModuleID());
    return false;
}

SvrModuleBase* SvrAppBase::GetMoudle(const std::string& strName)
{
    SvrModuleBase** p = _mapModule.find(strName);
    if (p != nullptr) return *p;
    return nullptr;
}

std::string SvrAppBase::_MakePidName()
{
    string strFilePath = g_strAppName;
    strFilePath = strFilePath + "_"+ std::to_string(GetAreanNo());
    strFilePath = strFilePath + "_" + std::to_string(GetIndex());
    strFilePath += ".pid";
    return strFilePath;
}

int32_t SvrAppBase::_Init()
{
    set_curr_time();
    set_tick_count();

#ifdef _MSC_VER
    LOG_SET_LEVEL(ELogTarget::Console, ELogLevel::Debug);
#else
    LOG_SET_LEVEL(ELogTarget::Console, ELogLevel::Warning);
#endif // _WIN

    LOG_SET_LEVEL(ELogTarget::File, ELogLevel::Debug);

    if (!LOG_MGR_INIT(SvrAppBase::GetAreanNo(), SvrAppBase::GetIndex(), false))
    {
        LOG_ERROR("log init error!!");
        return false;
    }

    LOG_CUSTOM("start", "name: %s, index:%d, version:%s begin..."
        , GetAppName().c_str(), GetIndex(), Version().c_str());

    srand((uint32_t) get_tick_count());
    LOG_DEBUG("start", "rand_max: %d", RAND_MAX);

    init_core_dumper();
    set_console_info(std::bind(&SvrAppBase::_OnSignalReload, this, std::placeholders::_1),
        std::bind(&SvrAppBase::_OnSignalStop, this, std::placeholders::_1));

    return Init();
}

bool SvrAppBase::_Run()
{
    _WritePidFile();
    LOG_CUSTOM("start", "%s start success!", g_strAppName.c_str());
    return Run();
}

void SvrAppBase::_Fini()
{
    LOG_CUSTOM("start", "%s end finish", g_strAppName.c_str());

    _LogicFini();
    _DBFini();
    _ResFini();

    std::this_thread::sleep_for(chrono::milliseconds(2000));
    Fini();
    LOG_MGR_UNINIT();
}

void SvrAppBase::_OnSignalReload(int32_t signal)
{
    Reload();
}

void SvrAppBase::_OnStop()
{
    LOG_INFO("Receive quit signal from console !");

    Stop();

    for (auto& item : _mapModule)
    {
        item->Stop();
    }
}

void SvrAppBase::_OnSignalStop(int32_t signal)
{
    _OnStop();
}

void SvrAppBase::_OnFrame(uint64_t qwTm)
{    
    set_curr_time();
    set_tick_count();

    uint64_t qwNow = get_tick_count();
    for (auto& item : _mapModuleFrame)
    {
        item->OnFrame(qwTm);
    }
    _qwPreMill = qwNow;
}

bool SvrAppBase::_DBInit()
{
    bool bRet = false;
    bool resume = false;
    for (auto& item : _mapModule)
    {
        item->SetResume(IsResume());
        if (item->IsResume() != IsResume())
            SetResume(item->IsResume());

        bRet = item->DBInit();
        ERROR_LOG_EXIT0(bRet);
    }
    return true;
Exit0:
    return false;
}

void SvrAppBase::_DBFini()
{
    for (auto& item : _mapModule)
    {
        item->DBFini();
    }
}

bool SvrAppBase::_ResInit()
{
    bool bRet = false;

    bRet = g_ModuleData.ResInit();
    ERROR_LOG_EXIT0(bRet);

    for (auto& item : _mapModule)
    {
        bRet = item->ResInit();
        ERROR_LOG_EXIT0(bRet);
    }

    return true;
Exit0:
    return false;
}

bool SvrAppBase::_ResAssembler()
{
    bool bRet = false;

    bRet = g_ModuleData.ResAssembler();
    ERROR_LOG_EXIT0(bRet);

    for (auto& item : _mapModule)
    {
        bRet = item->ResAssembler();
        ERROR_LOG_EXIT0(bRet);
    }
    return true;
Exit0:
    return false;
}

void SvrAppBase::_ResFini()
{
    for (auto& item : _mapModule)
    {
        item->ResFini();
    }
}

bool SvrAppBase::_LogicInit()
{
    bool bRet = false;
    for (auto& item : _mapModule)
    {
        bRet = item->LogicInit();
        ERROR_LOG_EXIT0(bRet);
    }
    return true;
Exit0:
    return false;
}

void SvrAppBase::_LogicFini()
{
    for (auto& item : _mapModule)
    {
        item->LogicFini();
    }
}

bool SvrAppBase::_PreReload()
{
    bool bRet = false;
    for (auto& item : _mapModule)
    {
        bRet = item->PreReload();
        ERROR_LOG_EXIT0(bRet);
    }
Exit0:
    return false;
}

bool SvrAppBase::_Reload()
{
    bool bRet = false;
    bRet = g_ModuleData.Reload();
    if (!bRet)
    {
        LOG_ERROR("Reload Error!!!");
        return false;
    }

    for (auto& item : _mapModule)
    {
        bRet = item->Reload();
        ERROR_LOG_EXIT0(bRet);
    }
    return bRet;
Exit0:
    return false;
}

bool SvrAppBase::_ReloadNotice()
{
    bool bRet = false;
    for (auto& item : _mapModule)
    {
        bRet = item->ReloadNotice();
        ERROR_LOG_EXIT0(bRet);
    }

    return true;
Exit0:
    return false;
}

bool SvrAppBase::IsResume()
{
    return g_pArgOpt->IsResume();
}

void SvrAppBase::SetResume(bool val)
{
    g_pArgOpt->SetResume(val);
}

void SvrAppBase::_WritePidFile()
{
    string strFilePath;

#ifdef _MSC_VER
    strFilePath = file::get_local_path(DEF_PID_PATH);
#else
    strFilePath = "/tmp/blooxr/";
#endif // _MSC_VER

    file::create_dir(strFilePath);

    strFilePath += _MakePidName();
    _strPidPath = strFilePath;

    file::write_file(_strPidPath, to_string(get_pid()));
}

void SvrAppBase::_OnTick(uint64_t qwTm)
{
    set_curr_time();
    set_tick_count();

    uint64_t qwNow = get_tick_count();
    for (auto& item : _mapModuleTick)
    {
        item->OnTick(qwTm);
    }

    _qwPreTick = qwNow;
}
