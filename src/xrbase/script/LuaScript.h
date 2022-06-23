/*
 *
 *      Author: venture
 */
#pragma once

#include <stdint.h>
#include <string>

class  LuaItem;
struct lua_State;

class LuaScript
{
public:
	LuaScript();
	~LuaScript();

	virtual bool Init(const std::string& szPath);
	void UnInit();

	int  CallFunc(const char* szGlobalName, const char* szFuncname, const char* szArgFormat, ...);

	void MainLoop(time_t tmTick);
	void StartStepGC(int nData);

	bool RegObj(LuaItem* pOjb, const char* szClassName);
	void UnRefObj(LuaItem* pOjb);

	lua_State* GetLuaState() const { return _pLuaState; }

private:
	void _AddLuaSearchPath(const std::string& szPath);
	bool _CompileScripts(const std::string& szPath);
	bool _Compile(const std::string& szPath);

private:
	lua_State*	_pLuaState = nullptr;
	std::string _szPath;
};