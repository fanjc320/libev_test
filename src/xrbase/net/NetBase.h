/*
 *
 *      Author: venture
 */
#pragma once
#include <stdint.h>

const uint32_t  DEF_MAX_BUFFER = 4 * 1024 * 1024; //4M
const int32_t	CHECK_DETECT_INTERVAL = 10;//10s

enum class EParseResult : int8_t
{
    OK = 0,
    INVALID_SIZE = 1,
    BUFFER_ERROR = 2,
    VERIFY_FAILED = 3,
    PUSH_FAILED = 4,
    MORE_FAILED = 5,
    CUS_KILL = 6,
    END = 7
};

enum class EConnState: int8_t
{
    UNKNOWN = 0,
    NET_ACCEPTED = 1,
    NET_CONNECTED = 2,
    NET_DISCONN = 3,
    LAYER_DISCONN = 4,
    LAYER_CONNECTED = 5,
    NET_NORECV = 6,
    LAYER_KILL = 7,
    NET_EXIT = 8, //进程退出
};

enum class EConnType : int8_t
{
    UNKNOWN = 0,
    ACCEPTED = 1,
    RECONN = 2,
};

enum class ECirType : int8_t
{
    ONE_ONE = 0, //单进单出
    MUL_ONE = 1, //多进单出
    MUL_MUL = 2, //多线程
    ONE = 3,     //网络层单线程处理
};

#ifndef _MSC_VER
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#else
#include <WinSock2.h>
#endif

#ifndef _MSC_VER
#define SP_THREAD_CALL
#else
#define SP_THREAD_CALL __stdcall
#endif

#ifndef SOCKET_T
// Definitions of common types
#if defined(_WIN64) || defined(WIN64)
typedef __int64          SOCKET_T;
#else
typedef int              SOCKET_T;
#endif
#endif

typedef uint16_t conn_oid_t;
const conn_oid_t INVALID_CONN_OID = 0xffff;
typedef uint16_t thread_oid_t;
const thread_oid_t INVALID_THREAD_OID = 0xff;
typedef uint32_t session_oid_t;
const session_oid_t INVALID_SESSION_OID = 0xffffffff;
typedef int8_t host_msg_t;
typedef uint8_t msg_id_t;