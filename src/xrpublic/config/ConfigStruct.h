#pragma  once
#include <string>
#include <map>
#include "dtype.h"
#include "config/ConstConfig.h"

using namespace std;
struct sIpPort
{
	string _ip = "";
	uint32_t _port = 0;

	bool IsValid() const
	{
		return (!_ip.empty() && _port != 0);
	}
};

///数据库配置
struct sDBSvrConfig
{
	bool IsValid() const
	{
        return true;
    }
};

// 世界服务器配置
struct sWorldSvrConfig
{
    std::vector<uint32_t> m_AreaList;
	bool IsValid() const
	{
		return m_AreaList.size() > 0;
	}
};

// 战斗中心服务器配置
struct sFCenterSvrConfig
{
    bool IsValid() const
    {
        return true;
    }
};

struct sCenterSvrConfig
{
    bool IsValid() const
    {
        return true;
    }
};


// 好友服务器配置
struct sFriendSvrConfig
{
	bool IsValid() const
	{
		return true;
	}
};

// log服务器配置
struct sLogSvrConfig
{
    bool    _IsHttpSdk = false;
	bool IsValid() const
	{
		return true;
	}
};

struct sFightSvrConfig
{
	sIpPort _IpPort;
	bool IsValid() const
	{
		return _IpPort.IsValid();
	}
};

struct sMallSvrConfig
{
	sIpPort _IpPort;
	bool IsValid() const
	{
		return _IpPort.IsValid();
	}
};

/////游戏服务器配置
struct sGameSvrConfig
{
    //游戏服务器监听信息
    bool        _gmOpen = false;

	bool IsValid() const
	{
		return true;
	}
};

////网关服务器配置
struct sGateSvrConfig
{
	sIpPort _IpPort;
	bool IsValid() const
	{
		return _IpPort.IsValid();
	}
};

////gmws配置信息
struct sHttpConfig
{
	string _strUrl;
	bool IsValid() const
	{
		return !_strUrl.empty();
	}
};