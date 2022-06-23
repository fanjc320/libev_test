#pragma once
#include "server/SvrAppBase.h"

class SvrApp : public SvrAppBase
{
public:
	virtual bool RegModule() override;
};