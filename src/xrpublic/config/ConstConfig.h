#pragma once
#include <string>
#include <map>
#include "dtype.h"
#include "ConfigStruct.h"
#include <set>

using namespace std;
class AppConfigBase;

struct sKafKa
{
	string _host = "";
	string _topic = "";
	string _project = "";
	bool IsValid() const
	{
		return !_host.empty() && !_topic.empty() && !_project.empty();
	}
};

struct sIpPortRedis
{
	string _ip = "";
	uint16_t _port = 0;
	string _pwd;

	bool IsValid() const
	{
		return (!_ip.empty() && _port != 0 && !_pwd.empty());
	}
};

struct sMongoSql
{
	string _host = "";
	string _name = "";
	string _user = "";
	string _pwd = "";
	string _auth = "";

	bool IsValid() const
	{
		return (!_host.empty() && !_name.empty()
			&& !_user.empty() && !_pwd.empty());
	}

	bool operator == (const sMongoSql& other)
	{
		return _host == other._host && _name == other._name
			&& _user == other._user && _pwd == other._pwd
			&& _auth == other._auth;
	}
};

struct sMySql
{
	string _host = "";
	string _name = "";
	string _user = "";
	string _pwd = "";
	int _port = 0;

	bool IsValid() const
	{
		return (!_host.empty() && !_user.empty() && _port > 0);
	}
};

// 排行榜服务器配置
struct sLogSvrConst
{
    uint32      _nLogDays = 30;
    uset<std::string> _setLogType;
	sKafKa      _Kafka;
    bool IsValid() const
    {
        return  _nLogDays > 0;
    }
};

struct sFightSvrConst
{
    uint32  _nMaxPlayer = 100;
    bool    _bUdp = false;
    uint32  _nMaxThead = 10;
    uint32  _nTimeoutsec = 15;

    bool IsValid()
    {
        return _nMaxPlayer > 0;
    }
};

/////游戏服务器配置
struct sGameSvrConst
{
    //游戏服务器监听信息
    uint32  _uiWorldChatInternal = 0;
    uint32  _uiReloginTime = 0;
    uint32  _createms = 1200;
	bool    _bDisableDiscountLimit = false;

    bool IsValid() const
    {
        return _uiWorldChatInternal > 0
            && _uiReloginTime > 0 && _createms;
    }
};

////网关服务器配置
struct sGateSvrConst
{
    //允许进游戏的最大值
    uint32  _nLimit = 0;
    //一秒验证的人数上限
    uint32  _nCheckMax = 0;
    uint32  _nTimeOut = 0;
    uint32  _nWaitcheck = 0;
    bool IsValid() const
    {
        return _nLimit > 0 && _nCheckMax > 0
            && _nTimeOut > 0 && _nWaitcheck > 0;
    }
};

struct sMongoDB
{
	sMongoSql    _GameMongo;
	sMongoSql    _SysMailMongo;
	sMongoSql    _NoticeMailMongo;
	sMongoSql    _MallMongo;
	sMongoSql    _AccountMongo;
    sMongoSql    _FriendMongo;

	bool IsValid() const
	{
		return _GameMongo.IsValid() && _SysMailMongo.IsValid()
			&& _NoticeMailMongo.IsValid() && _MallMongo.IsValid()
			&& _AccountMongo.IsValid()
            && _FriendMongo.IsValid();
	}
};

struct sMySqlDB
{
	sMySql _AccMySql;
	sMySql _LogMySql;

	bool IsValid() const
	{
		return _AccMySql.IsValid() && _LogMySql.IsValid();
	}
};

struct sCacheRedis
{
	sIpPortRedis _config;
    sIpPortRedis _mq;
	bool IsValid() const
	{
		return _config.IsValid() && _mq.IsValid();
	}
};

class ConstConfig
{
public:
    ConstConfig();

    bool   Init(AppConfigBase* pConfigXml = nullptr);
    //1:安卓 2：IOS
    uint32 GetPlatid();

    //是否内存缓存玩家
    bool IsCached();

    //是否是单一好友服
    bool IsOneFriend();

    //带宽 Mb/s
    uint32 GetBandwidth();

    sLogSvrConst GetLogSvrConst();
    sFightSvrConst GetFightSvrConst();
    sGameSvrConst GetGameSvrConst();
    sGateSvrConst GetGateSvrConst();
    std::string   GetUrlPath();

	//mongo
	sMongoDB GetMongoConfig(uint32 id);
	//mysql
	sMySqlDB GetMySqlConfig(uint32 id);
	//redis
	sCacheRedis GetCacheRedis(uint32 id);

private:
	void ReadKafkaConf(sKafKa& sql, string strKey);
	void ReadMongoConf(sMongoSql& sql, string strKey);
	void ReadMysqlConf(sMySql& sql, string strKey);
    void ReadRedisConf(sIpPortRedis& sql, string strKey);

private:
	AppConfigBase* m_pConfig = nullptr;
	bool m_bCached = false;

	uint32 _bandwidth = 0;//带宽 Mb/s
	uint32 _platid = 0;// 1:安卓 2：IOS
};