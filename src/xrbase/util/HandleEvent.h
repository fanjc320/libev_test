#pragma once
#include <vector>
#include <unordered_map>
#include <stdint.h>

#include "stl/map_list.h"
#include "log/LogMgr.h"
#include "math/math_basic.h"

//////////////////////////////////////////////////////////////////////////
// H文件
// DECLARE_EVENT()
// cpp文件
// BEGIN_EVENT(CTest)
// EVENT(1, 2, &CTest::OnLogin)
// EVENT(1, 1, &CTest::OnLoginOut)
// END_EVENT()
//////////////////////////////////////////////////////////////////////////
// H文件
// DECLARE_EVENT_STATIC()
// cpp文件
// BEGIN_EVENT_STATIC(CTestStatic)
// EVENT(1, 2, &CTestStatic::OnLogin)
// EVENT(1, 1, &CTestStatic::OnLoginOut)
// END_EVENT_STATIC()
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <iostream>

#define DECLARE_EVENT_STATIC2(P1, P2) static int OnEvent(uint32_t id, P1 p1, P2 p2);

#define BEGIN_EVENT_STATIC2(T, P1, P2) \
int T::OnEvent(uint32_t id, P1 p1, P2 p2) \
{\
    static EventStaticT<P1, P2> ev = {\

#define END_EVENT_STATIC2() }; \
    return ev.OnEvent(id, p1, p2);\
}

#define DECLARE_EVENT(P1, P2) int OnEvent(uint32_t id, P1 p1, P2 p2);

#define BEGIN_EVENT(T, P1, P2) \
int T::OnEvent2(uint32_t id, P1 p1, P2 p2) \
{\
    static EventT<T, P1, P2> ev = {\

#define END_EVENT2() }; \
    return ev.OnEvent(this, id, p1, p2);\
}

#define DECLARE_EVENT_STATIC1(P1) static int OnEvent(uint32_t id, P1 p1);

#define BEGIN_EVENT_STATIC1(T, P1) \
int T::OnEvent(uint32_t id, P1 p1) \
{\
    static EventStaticT<P1> ev = {\

#define END_EVENT_STATIC1() }; \
    return ev.OnEvent(id, p1);\
}

#define DECLARE_EVENT1(P1) int OnEvent(uint32_t id, P1 p1);

#define BEGIN_EVENT1(T, P1) \
int T::OnEvent(uint32_t id, P1 p1) \
{\
    static EventT<T, P1> ev = {\

#define END_EVENT1() }; \
    return ev.OnEvent(this, id, p1);\
}

//////////////////////////////////////////////////////////////////////////
////下面的需要在H头文件中定义全部
//////////////////////////////////////////////////////////////////////////
#define BEGIN_EVENT_ALL_STATIC() \
template<class... _ArgTypes> \
static int OnEvent(uint32_t id, _ArgTypes... _Args) \
{\
	static EventStaticT<_ArgTypes...> ev = { \

#define END_EVENT_ALL_STATIC() }; \
    return ev.OnEvent(id, _Args...);\
}

#define BEGIN_EVENT_ALL(T) \
template<class... _ArgTypes> \
int OnEvent(uint32_t id, _ArgTypes... _Args) \
{\
	static EventT<T, _ArgTypes...> ev = { \

#define END_EVENT_ALL() }; \
    return ev.OnEvent(this, id, _Args...);\
}

//////////////////////////////////////////////////////////////////////////

#define EVENT(mod, id, func) {MAKE_UINT32(mod, id), func },

template<class... _ArgTypes>
class EventStaticT
{
public:
	typedef bool(*Func)(_ArgTypes...);

	EventStaticT()
	{

	}
	EventStaticT(std::initializer_list<std::pair<uint32_t, Func>> _Ilist)
	{
		for (std::pair<uint32_t, Func> itr : _Ilist)
		{
			Reg(itr.first, itr.second);
		}
	}

	void Reg(uint32_t id, Func func)
	{
		if (id <= UINT8_MAX)
		{
			if (id >= _func.size())
			{
				_func.reserve(id + 1);
				_func.resize(id + 1);
			}
			_func[id] = func;
		}

		_func_map[id] = func;
	}

	Func Get(uint32_t id)
	{
		if (id >= 0 && id < _func.size()) return _func[id];
		auto itr = _func_map.find(id);
		if (itr == _func_map.end()) return nullptr;
		return itr->second;
	}

	int OnEvent(uint32_t id, _ArgTypes... _Args)
	{
		Func pFunc = Get(id);
		if (pFunc != nullptr)
		{
			return (*pFunc)(_Args...);
		}
		else
		{
			LOG_WARNING("error id: %d", id );
		}

		return -1;
	}

private:
	std::vector<Func> _func;
	std::unordered_map<uint32_t, Func> _func_map;
};

template<typename T, class... _ArgTypes>
class EventT
{
public:
	typedef bool(T::*Func)(_ArgTypes...);

	EventT()
	{

	}

	EventT(std::initializer_list<std::pair<uint32_t, Func>> _Ilist)
	{
		for (std::pair<uint32_t, Func> itr : _Ilist)
		{
			Reg(itr.first, itr.second);
		}
	}

	void Reg(uint32_t id, Func func)
	{
		if (id <= UINT8_MAX)
		{
			if (id >= _func.size())
			{
				_func.reserve((size_t)id + 1);
				_func.resize((size_t)id + 1);
			}
			_func[id] = func;
		}

		_func_map[id] = func;
	}

	Func Get(uint32_t id)
	{
		if (id >= 0 && id < _func.size()) return _func[id];
		auto itr = _func_map.find(id);
		if (itr == _func_map.end()) return nullptr;
		return itr->second;
	}

	int OnEvent(T* base, uint32_t id, _ArgTypes... _Args)
	{
		Func pFunc = Get(id);
		if (pFunc != nullptr)
		{
			return (base->*pFunc)(_Args...);
		}
		else
		{
			LOG_WARNING("error id: %d", id);
		}

		return -1;
	}

private:
	std::vector<Func> _func;
	std::unordered_map<uint32_t, Func> _func_map;
};