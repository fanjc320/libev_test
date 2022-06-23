/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "SvrProgMqHandle.h"
#include "dtype.h"

SvrProgMqHandle::SvrProgMqHandle()
{
}

SvrProgMqHandle::~SvrProgMqHandle()
{
}

void SvrProgMqHandle::FreeReconT()
{
    for (auto& p : _mapMq)
    {
        SAFE_DELETE(p);
    }
    _mapMq.clear();
}

SvrProgMq* SvrProgMqHandle::GetRecon(uint32_t key)
{
    SvrProgMq** p = _mapMq.find(key);
    if (p == nullptr) return nullptr;
    return *p;
}

bool SvrProgMqHandle::AddRecon(SvrProgMq* p)
{
    if (p == nullptr) return false;
    ++key;
    p->SetKey(key);
    return _mapMq.insert(p->GetKey(), p);
}

void SvrProgMqHandle::Clear()
{
    _mapMq.clear();
}
