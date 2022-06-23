/*
 *
 *      Author: venture
 */
#pragma once

#include <stdio.h>
#include <string.h>
#include <string>
#include "net/NetBase.h"

struct NetSetting
{
    thread_oid_t ioNum = INVALID_THREAD_OID;
    conn_oid_t	 connsOfIo = INVALID_CONN_OID;

    uint32_t	 inMaxSize = 256 * 1024;
    uint32_t	 outMaxSize = 256 * 1024;
    int32_t      detectSec = CHECK_DETECT_INTERVAL * 12;
    int32_t      secRecvLimit = 1024 * 1024;
    //消息队列的方式
    ECirType	 cir = ECirType::ONE_ONE;
    ECirType     CirType() const { return cir; }

    //listen  thread--------->
    std::string  ip;
    uint32_t     port = 0;
    uint32_t     acceptMaxSize = DEF_MAX_BUFFER;
    //<----------listen  thread

    //recon  thread--------->
    uint32_t     reconSec = 1;
    //<----------recon  thread

    bool IsValid() const
    {
        return ioNum > 0 && connsOfIo > 0;
    }

    void Init(thread_oid_t nIos, conn_oid_t	ConnsOfIo
        , uint32_t aptMaxSize, uint32_t inMax
        , uint32_t outMax, int32_t nSecRecv = 1024 * 1024
        , int32_t nDetectSecs = CHECK_DETECT_INTERVAL * 12)
    {
        ioNum = nIos;
        connsOfIo = ConnsOfIo;
        acceptMaxSize = aptMaxSize;
        inMaxSize = inMax;
        outMaxSize = outMax;

        if (nDetectSecs < CHECK_DETECT_INTERVAL * 2)
            detectSec = CHECK_DETECT_INTERVAL * 2;
        else
            detectSec = nDetectSecs;

        secRecvLimit = nSecRecv;
    }
};