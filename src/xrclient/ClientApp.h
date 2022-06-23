#pragma once
#include "app/SvrApp.h"
#include "util/SingleTon.h"

class ClientArgOpt;
class ClientApp : public SvrApp, public SingleTon<ClientApp>
{
    ClientApp() = default;
    ~ClientApp();
    friend SingleTon;

public:
    virtual int32_t Init(uint64_t millFrame = 20)  override;
    virtual bool Arg(int argc, char* argv[]) override;
    static ClientArgOpt* GetClientOpt();

    virtual bool RegModule() override;
    virtual void Stop()  override;
    virtual void Fini()  override;
    virtual bool Reload()  override;
};

