#pragma once
#include <stdint.h>
#include <string>

class SvrAppBase;

class SvrModuleBase
{
public:
    SvrModuleBase(const std::string& moduleid = "");

    //  ���ݳ�ʼ��
    virtual bool DBInit() { return true; }
    virtual void DBFini() { }
    
    // ��Դ��ʼ��
    virtual bool ResInit() { return true; }

    // ��Դ����
    virtual bool ResAssembler() { return true; }
    virtual void ResFini() { }

    // �߼���ʼ��: �����Ҫ������ʼ��Э��ע��
    // timer�ص�, �ȼ������ݺ���Դ, ��׼���߼�
    virtual bool LogicInit() { return true; }

    virtual void OnFrame(uint64_t qwTm) { }
    virtual void OnTick(uint64_t qwTm) { } //һ����һ��

    virtual bool PreReload() { return true; }

    // ��Դ��reload����
    virtual bool Reload() { return true; }
    // ��reload֮����Ҫ��һЩ���֪ͨ��ϵͳ֪ͨ
    virtual bool ReloadNotice() { return true; }
    // reload֮ǰ�Ĵ������������, ����, �����

    virtual void Stop() { }
    virtual void LogicFini() { }

    const std::string& GetModuleID();

    bool IsResume() const { return _resume; }
    void SetResume(bool val) { _resume = val; }

private:
    bool _resume = false;
    std::string _id;
};

