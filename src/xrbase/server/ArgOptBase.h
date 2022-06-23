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

class ArgOptBase
{
public:
	ArgOptBase() { }
	~ArgOptBase() {}

	virtual bool Argv(int argc, char* argv[]) = 0;

	uint32_t GetIndex() const { return _index; }
	const std::string& GetConfig() const { return _config; }
	void SetConfig(const std::string& val) { _config = val; }
	const std::string& GetMode() const { return _mode; }
	uint32_t GetAreanNo() const { return _areano; }
	const std::string& GetAlias() const { return _alias; }
	bool  IsResume() const { return _resume; }
	void  SetResume(bool val) { _resume = val; }

protected:
	uint32_t          _index = 0;
	uint32_t          _areano = 0;
	std::string       _config;
	std::string       _mode;
	std::string       _alias;
	bool			  _resume = false;
};

