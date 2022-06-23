#pragma once
#include <stdint.h>
#include <string>

class SvrAppBase;

class SvrModuleBase
{
public:
    SvrModuleBase(const std::string& moduleid = "");

    //  数据初始化
    virtual bool DBInit() { return true; }
    virtual void DBFini() { }
    
    // 资源初始化
    virtual bool ResInit() { return true; }

    // 资源重组
    virtual bool ResAssembler() { return true; }
    virtual void ResFini() { }

    // 逻辑初始化: 这个主要用来初始化协议注册
    // timer回调, 先加载数据和资源, 再准备逻辑
    virtual bool LogicInit() { return true; }

    virtual void OnFrame(uint64_t qwTm) { }
    virtual void OnTick(uint64_t qwTm) { } //一毫秒一次

    virtual bool PreReload() { return true; }

    // 资源的reload操作
    virtual bool Reload() { return true; }
    // 在reload之后，需要做一些玩家通知，系统通知
    virtual bool ReloadNotice() { return true; }
    // reload之前的处理，可以做检查, 备份, 清理等

    virtual void Stop() { }
    virtual void LogicFini() { }

    const std::string& GetModuleID();

    bool IsResume() const { return _resume; }
    void SetResume(bool val) { _resume = val; }

private:
    bool _resume = false;
    std::string _id;
};

