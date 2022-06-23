#include <string.h>
#include <map>
#include "ConstConfig.h"
#include "log/LogMgr.h"
#include "platform/file_func.h"
#include "const.h"
#include "server/AppConfigBase.h"
#include "string/str_format.h"
#include "string/str_split.h"

using namespace std;

ConstConfig::ConstConfig()
{

}

bool ConstConfig::Init(AppConfigBase* pConfigXml)
{
    if (pConfigXml == nullptr)
    {
        std::string strName = file::get_local_path(DEF_CONST_CFG_FILE);
        if (!file::is_exsit_file(strName)) return false;
        m_pConfig = new AppConfigBase();

        return m_pConfig->Load(strName);
    }
    m_pConfig = pConfigXml;
    return true;
}

uint32 ConstConfig::GetPlatid()
{
    if (_platid == 0)
        _platid = m_pConfig->ReadAttrInt("platid");
    if (_platid == 0)
    {
        LOG_ERROR("platid is 0!");
    }
    return _platid;
}

bool ConstConfig::IsCached()
{
    if (!m_bCached)
        m_bCached = m_pConfig->ReadAttrBool("cached");
    return m_bCached;
}

bool ConstConfig::IsOneFriend()
{
    return m_pConfig->ReadAttrBool("friend");
}

uint32 ConstConfig::GetBandwidth()
{
    if (_bandwidth == 0)
        _bandwidth = m_pConfig->ReadAttrInt("bandwidth");
    return _bandwidth;
}

sLogSvrConst ConstConfig::GetLogSvrConst()
{
    sLogSvrConst svrLogConfig;
    svrLogConfig._nLogDays = m_pConfig->ReadAttrInt("xrlog:logdays");
    string strLogType = m_pConfig->ReadAttrStr("xrlog:logtype");
    str_split_set(strLogType, ',', svrLogConfig._setLogType);

	ReadKafkaConf(svrLogConfig._Kafka, "xrlog:log_kafka:0");

    if (svrLogConfig._nLogDays == 0)
    {
        svrLogConfig._nLogDays = 15;
        LOG_WARNING("LogDays is 0, setdefult 15 days");
    }

    if (!svrLogConfig.IsValid())
    {
        LOG_ERROR("Config Error!");
    }
    return svrLogConfig;
}

sFightSvrConst ConstConfig::GetFightSvrConst()
{
    sFightSvrConst svrConfig;

    svrConfig._nMaxPlayer = m_pConfig->ReadAttrInt("xrfight:maxplayer");
    svrConfig._bUdp = m_pConfig->ReadAttrBool("xrfight:udp");
    svrConfig._nMaxThead = m_pConfig->ReadAttrInt("xrfight:maxthread");
    svrConfig._nTimeoutsec = m_pConfig->ReadAttrInt("xrfight:timeout");

    if (svrConfig._nMaxThead <= 0)
    {
        svrConfig._nMaxThead = 15;
    }
    if (svrConfig._nTimeoutsec <= 0)
    {
        svrConfig._nTimeoutsec = 15;
    }

    if (!svrConfig.IsValid())
    {
        LOG_ERROR("Config Error!");
    }

    return svrConfig;
}

/************************************************************************/
/* 游戏服务器配置信息                                                                     */
/************************************************************************/
sGameSvrConst ConstConfig::GetGameSvrConst()
{
    sGameSvrConst svrConfig;
    svrConfig._uiWorldChatInternal = m_pConfig->ReadAttrInt("xrgame:worldchatinternal");
    svrConfig._uiReloginTime = m_pConfig->ReadAttrInt("xrgame:relogintime");
    svrConfig._createms = m_pConfig->ReadAttrInt("xrgame:createms");
	svrConfig._bDisableDiscountLimit = m_pConfig->ReadAttrBool("xrgame:DisableDiscountLimit");
    if (svrConfig._createms == 0)
    {
        svrConfig._createms = 1200;
        LOG_WARNING("createms default 1200ms");
    }

    if (!svrConfig.IsValid())
    {
        LOG_ERROR("Config Error!");
    }

    return svrConfig;
}

/************************************************************************/
/* 网关服务器配置信息                                                                     */
/************************************************************************/
sGateSvrConst ConstConfig::GetGateSvrConst()
{
    sGateSvrConst svrConfig;
    svrConfig._nLimit = m_pConfig->ReadAttrInt("xrgame:limit");
    svrConfig._nCheckMax = m_pConfig->ReadAttrInt("xrgame:checkmax");
    svrConfig._nTimeOut = m_pConfig->ReadAttrInt("xrgame:timeout");
    svrConfig._nWaitcheck = m_pConfig->ReadAttrInt("xrgame:waitcheck");

    if (!svrConfig.IsValid())
    {
        LOG_ERROR("Config Error!");
    }

    return svrConfig;
}

/************************************************************************/
/* gmws接口地址                                                                     */
/************************************************************************/
std::string ConstConfig::GetUrlPath()
{
    std::string strPath = m_pConfig->ReadAttrStr("http:addr");

    if (strPath.empty())
    {
        LOG_ERROR("GmwsPath Error!");
    }

    return strPath;
}

sMongoDB ConstConfig::GetMongoConfig(uint32 id)
{
	sMongoDB mongoConfig;
	ReadMongoConf(mongoConfig._GameMongo, "mongolist:" + std::to_string(id) + ":game_db");
	ReadMongoConf(mongoConfig._SysMailMongo, "mongolist:" + std::to_string(id) + ":sys_mail_db");
	ReadMongoConf(mongoConfig._NoticeMailMongo, "mongolist:" + std::to_string(id) + ":notice_mail_db");
	ReadMongoConf(mongoConfig._MallMongo, "mongolist:" + std::to_string(id) + ":mall_db");
	ReadMongoConf(mongoConfig._AccountMongo, "mongolist:" + std::to_string(id) + ":account_db");
    ReadMongoConf(mongoConfig._FriendMongo, "mongolist:" + std::to_string(id) + ":friend_db");

    if (!mongoConfig.IsValid())
    {
        LOG_ERROR("Config Error! id:%u", id);
    }

	return mongoConfig;
}

sMySqlDB ConstConfig::GetMySqlConfig(uint32 id)
{
	sMySqlDB mysqlConfig;
	ReadMysqlConf(mysqlConfig._AccMySql, "mysqllist:" + std::to_string(id) + ":account_mysql");
	ReadMysqlConf(mysqlConfig._LogMySql, "mysqllist:" + std::to_string(id) + ":log_mysql");

    if (!mysqlConfig.IsValid())
    {
        LOG_ERROR("Config Error! id:%u", id);
    }
	return mysqlConfig;
}

sCacheRedis ConstConfig::GetCacheRedis(uint32 id)
{
	sCacheRedis redisConfig;
    ReadRedisConf(redisConfig._config, "redislist:" + std::to_string(id) + ":redis_config");
    ReadRedisConf(redisConfig._mq, "redislist:" + std::to_string(id) + ":redis_mq");

    if (!redisConfig.IsValid())
    {
        LOG_ERROR("Config Error! id:%u", id);
    }
	return redisConfig;
}

void ConstConfig::ReadKafkaConf(sKafKa& sql, string strKey)
{
	sql._host = m_pConfig->ReadAttrStr(strKey + ":host");
	sql._topic = m_pConfig->ReadAttrStr(strKey + ":topic");
	sql._project = m_pConfig->ReadAttrStr(strKey + ":project");
}

void ConstConfig::ReadMongoConf(sMongoSql& sql, string strKey)
{
	sql._host = m_pConfig->ReadAttrStr(strKey + ":host");
	sql._name = m_pConfig->ReadAttrStr(strKey + ":name");
	sql._pwd = m_pConfig->ReadAttrStr(strKey + ":password");
	sql._user = m_pConfig->ReadAttrStr(strKey + ":user");
	sql._auth = m_pConfig->ReadAttrStr(strKey + ":auth");
}

void ConstConfig::ReadMysqlConf(sMySql& sql, string strKey)
{
	sql._host = m_pConfig->ReadAttrStr(strKey + ":host");
	sql._name = m_pConfig->ReadAttrStr(strKey + ":name");
	sql._pwd = m_pConfig->ReadAttrStr(strKey + ":password");
	sql._user = m_pConfig->ReadAttrStr(strKey + ":user");
	sql._port = m_pConfig->ReadAttrInt(strKey + ":port");
}

void ConstConfig::ReadRedisConf(sIpPortRedis& sql, string strKey)
{
    sql._ip = m_pConfig->ReadAttrStr(strKey + ":IP");
    sql._pwd = m_pConfig->ReadAttrStr(strKey + ":password");
    sql._port = m_pConfig->ReadAttrInt(strKey + ":port");
}
