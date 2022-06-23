/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "sock_opt.h"
#ifndef _MSC_VER
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "log/LogMgr.h"
#include "net/NetBase.h"

namespace sock_opt
{
    bool SetKeepALive(SOCKET_T fd)
    {
        const int32_t keepAlive = 1; //设定KeepAlive
        const int32_t keepIdle = 60; //开始首次KeepAlive探测前的TCP空闭时间
        const int32_t keepInterval = 5; //两次KeepAlive探测间的时间间隔
        const int32_t keepCount = 10; //判定断开前的KeepAlive探测次数
        const int32_t nodelay = 1;

        if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*) &keepAlive,
            sizeof(keepAlive)) == -1)
        {
            LOG_ERROR("setsockopt SO_KEEPALIVE error!");
        }

#ifndef _MSC_VER
        if (setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (char*) &keepIdle,
            sizeof(keepIdle)) == -1)
        {
            LOG_ERROR("setsockopt TCP_KEEPIDLE error!");
        }

        if (setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (char*) &keepInterval,
            sizeof(keepInterval)) == -1)
        {
            LOG_ERROR("setsockopt TCP_KEEPINTVL error!");
        }

        if (setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (char*) &keepCount,
            sizeof(keepCount)) == -1)
        {
            LOG_ERROR("setsockopt TCP_KEEPCNT error!");
        }
#endif
        if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*) &nodelay, sizeof(nodelay)) == -1)
        {
            LOG_ERROR(" setsockopt  client:%d, err:%d, %s", fd, errno, strerror(errno));
            return false;
        }

        return true;
    }

    bool SetRcvSize(SOCKET_T fd, int32_t size)
    {
        int32_t rcvbuf_len;
#ifdef _MSC_VER
        int32_t len = sizeof(rcvbuf_len);
#else
        uint32_t len = sizeof(rcvbuf_len);
#endif // _MSC_VER

        if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*) &rcvbuf_len, &len)
            < 0)
        {
            LOG_ERROR("getsockopt: ");
            return false;
        }
        if (rcvbuf_len >= size) return true;
        rcvbuf_len = size;
        if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*) &rcvbuf_len, len)
            < 0)
        {
            LOG_ERROR("setsockopt: ");
            return false;
        }
        return true;
    }

    bool SetSndSize(SOCKET_T fd, int32_t size)
    {
        int32_t rcvbuf_len;

#ifdef _MSC_VER
        int32_t len = sizeof(rcvbuf_len);
#else
        uint32_t len = sizeof(rcvbuf_len);
#endif // _MSC_VER

        if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*) &rcvbuf_len, &len)
            < 0)
        {
            LOG_ERROR("getsockopt: ");
            return false;
        }

        if (rcvbuf_len >= size) return true;
        rcvbuf_len = size;
        if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*) &rcvbuf_len, len)
            < 0)
        {
            LOG_ERROR("setsockopt: ");
            return false;
        }
        return true;
    }

    bool SetBufSize(SOCKET_T fd, int32_t size /*= 65536*/)
    {
        SetRcvSize(fd, size);
        SetSndSize(fd, size);
        return true;
    }

    bool SetCloseExec(SOCKET_T fd)
    {
#ifndef _MSC_VER
        int flags = fcntl(fd, F_GETFD);
        if (flags < 0)
        {
            LOG_ERROR("SetColseExec");
            return false;
        }
        flags |= FD_CLOEXEC;
        if (fcntl(fd, F_SETFD, flags) < 0)
        {
            LOG_ERROR("SetColseExec");
            return false;
        }
#endif // _MSC_VER
        return true;
    }

    bool SetNoDelay(SOCKET_T fd)
    {
        const int32_t nodelay = 1;

        if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*) &nodelay, sizeof(nodelay)) == -1)
        {
            LOG_ERROR(" setsockopt  client:%d, err:%d, %s", fd, errno, strerror(errno));
            return false;
        }
        return true;
    }

    bool SetNoBlock(SOCKET_T fd)
    {
#ifdef _MSC_VER
        {
            u_long nonblocking = 1;
            if (ioctlsocket(fd, FIONBIO, &nonblocking) == SOCKET_ERROR)
            {
                return false;
            }
        }
#else
        {
            int flags;
            if ((flags = fcntl(fd, F_GETFL, nullptr)) < 0)
            {
                return false;
            }
            if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
            {
                return false;
            }
        }
#endif
        return true;
    }

    bool SetSockReuseable(SOCKET_T fd)
    {
        int one = 1;    
#ifndef _MSC_VER
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*) &one,
            sizeof(one)) == 0)
            return true;
        else
            return false;
#else
        return true;
#endif
    }

    bool CloseSocket(SOCKET_T fd)
    {
#ifdef _MSC_VER
        return ::closesocket(fd);
#else
        return ::close(fd);
#endif
    }

    int inet_ntop4(const unsigned char* src, char* dst, size_t size)
    {
        static const char fmt[] = "%u.%u.%u.%u";
        char tmp[16];
        int l;

        l = snprintf(tmp, sizeof(tmp), fmt, src[0], src[1], src[2], src[3]);
        if (l <= 0 || (size_t) l >= size)
        {
            return -1;
        }
        strncpy(dst, tmp, size);
        dst[size - 1] = '\0';
        return 0;
    }

    std::string Inet_Ip(sockaddr& addr)
    {
        uint32_t ip = ((sockaddr_in*) (&addr))->sin_addr.s_addr;
        return Inet_Ip(ip);
    }

    std::string Inet_Ip(uint32_t ip)
    {
        char chIp[16] = { 0 };
        struct in_addr in;
        memcpy(&in, &ip, sizeof(ip));

        if (inet_ntop4((const unsigned char*) &in, chIp, 16) != 0)
            return "0.0.0.0";

        return (std::string) (chIp);
    }

    std::string GetLocalIpAddress()
    {
        char local[255] = { 0 };
        gethostname(local, sizeof(local));
        hostent* ph = gethostbyname(local);
        if (ph == nullptr)
            return "0.0.0.0";
        in_addr addr;
        memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); // 这里仅获取第一个ip
        std::string localIP;
        localIP.assign(inet_ntoa(addr));
        return localIP;
    }
}
