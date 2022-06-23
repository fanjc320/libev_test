/*
 *
 *      Author: venture
 */
#pragma once

#include <iostream>

#ifndef _MSC_VER
#include <getopt.h>
#else
#include "platform/wingetopt.h"
#endif
#include "log/LogMgr.h"
#include "server/ArgOptBase.h"

class SvrArgOpt : public ArgOptBase
{
public:
	SvrArgOpt() { }
	~SvrArgOpt() {}

	void Man();
	bool Argv(int argc, char* argv[]);
};

