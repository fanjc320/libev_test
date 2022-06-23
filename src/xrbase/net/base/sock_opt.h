/*
 *
 *      Author: venture
 */
#pragma once

#include <stdint.h>
#include <string>
#include "net/NetBase.h"

namespace sock_opt
{
    const int MAX_SIZE_64K = 64 * 1024;

    bool SetKeepALive(SOCKET_T fd);
    bool SetRcvSize(SOCKET_T fd, int32_t size = MAX_SIZE_64K);
    bool SetSndSize(SOCKET_T fd, int32_t size = MAX_SIZE_64K);
    bool SetBufSize(SOCKET_T fd, int32_t size = MAX_SIZE_64K);
    bool SetCloseExec(SOCKET_T fd);
    bool SetNoDelay(SOCKET_T fd);
    bool SetNoBlock(SOCKET_T fd);
    bool SetSockReuseable(SOCKET_T fd);
    bool CloseSocket(SOCKET_T fd);

    std::string Inet_Ip(sockaddr& addr);
    std::string Inet_Ip(uint32_t ip);
    std::string GetLocalIpAddress();
}

