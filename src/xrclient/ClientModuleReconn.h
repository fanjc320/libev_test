#pragma once
#include "dtype.h"
#include "server/SvrModuleBase.h"
#include "util/SingleTon.h"
class ClientModuleReconn : public SvrModuleBase, public SingleTon<ClientModuleReconn>
{
    ClientModuleReconn() = default;
    friend SingleTon;
public:

    virtual bool LogicInit();
    virtual void LogicFini();

    virtual void OnFrame(uint64_t qwTm) override;
    virtual void OnTick(uint64_t qwTm) override; //“ª∫¡√Î“ª¥Œ
};

