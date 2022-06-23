#pragma once
#include "app/SvrApp.h"
#include "util/SingleTon.h"
class XrHttpApp : public SvrApp, public SingleTon<XrHttpApp>
{
    XrHttpApp() = default;
    ~XrHttpApp();
    friend SingleTon;
public:

    virtual int32_t Init(uint64_t millFrame = 20)  override;
    virtual bool RegModule() override;
    virtual void Stop()  override;
    virtual void Fini()  override;
    virtual bool Reload()  override;
};

