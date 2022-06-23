#pragma once
#include "app/SvrApp.h"
#include "util/SingleTon.h"
class XrGateApp : public SvrApp, public SingleTon<XrGateApp>
{
    XrGateApp() = default;
    ~XrGateApp();
    friend SingleTon;
public:

    virtual int32_t Init(uint64_t millFrame = 20)  override;
    virtual bool RegModule() override;
    virtual void Stop()  override;
    virtual void Fini()  override;
    virtual bool Reload()  override;
};

