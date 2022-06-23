#pragma once
#include "app/SvrApp.h"
#include "util/SingleTon.h"

class XrLogApp : public SvrApp, public SingleTon<XrLogApp>
{
    XrLogApp() = default;
    ~XrLogApp();
    friend SingleTon;

public:

    virtual int32_t Init(uint64_t millFrame = 20)  override;
    virtual bool RegModule() override;
    virtual void Stop()  override;
    virtual void Fini()  override;
    virtual bool Reload()  override;
};

