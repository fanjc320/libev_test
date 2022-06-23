#pragma  once

#include "dtype.h"
#include "server/SvrModuleBase.h"
#include "time/TimeCount.h"
#include "util/SingleTon.h"

class ModuleShm : public SvrModuleBase, public SingleTon<ModuleShm>
{
    ModuleShm();
    friend SingleTon;
public:
    virtual bool DBInit() override;

    virtual bool LogicInit() override;
    virtual void LogicFini() override;

    virtual bool Reload() override;
    virtual bool ReloadNotice() override;
    virtual bool PreReload() override;
    virtual void Stop() override;

    virtual void OnFrame(uint64_t qwTm) override;
private:
    TimeCount<180> _tmGC;
};