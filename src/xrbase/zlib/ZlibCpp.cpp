/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "zlib/ZlibCpp.h"
#include "zlib.h"
#include <stdio.h>
#include "log/LogMgr.h"

ZlibCpp::ZlibCpp()
{
}

ZlibCpp::~ZlibCpp()
{
}

bool ZlibCpp::Encode(const string& src, string& zipbuf)
{
	if (src.size() == 0) return true;

    try
    {
	    int err;
	    unsigned long size = (unsigned long)src.size();

	    unsigned long zipedsize = compressBound(size);
	    zipbuf.clear();
	    zipbuf.resize(zipedsize);

	    //压缩 ，第一个参数是目标缓冲区，存放压缩后的数据
	    //第二个参数是压缩后的数据大小
	    //第三个参数是源数据缓冲区
	    //第四个参数是源缓冲区的大小
	    err = compress((unsigned char*)zipbuf.c_str(), &zipedsize, (const unsigned char*)src.c_str(), size);
	    if (err != Z_OK)
		    return false;

	    zipbuf.resize(zipedsize);
        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("%s", e.what());
        return false;
    }
	return false;
}

bool ZlibCpp::Encode(const char* src, unsigned long srclen, char* zipbuf, unsigned long& ziplen)
{
    if (srclen == 0) return true;

    try
    {
        int err;
        unsigned long size = (unsigned long)srclen;
        unsigned long zipedsize = compressBound(size);
        //压缩 ，第一个参数是目标缓冲区，存放压缩后的数据
        //第二个参数是压缩后的数据大小
        //第三个参数是源数据缓冲区
        //第四个参数是源缓冲区的大小
        err = compress((unsigned char*)zipbuf, &zipedsize, (const unsigned char*)src, size);
        if (err != Z_OK)
            return false;
        ziplen = zipedsize;

        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("%s", e.what());
        return false;
    }
    return false;
}

bool ZlibCpp::EncodeFile(const string& filename, string& zipbuf)
{
    string strSrc;
    if (!LoadBuf(filename, strSrc)) return false;
    return Encode(strSrc, zipbuf);
}

bool ZlibCpp::Decode(const string& zipbuf, string& src, unsigned long srclen)
{
	if (zipbuf.size() == 0) return true;

    try
    {
        int err;
        unsigned long ziplen = (unsigned long)zipbuf.size();
        src.clear();
        src.resize(srclen);

        unsigned long size = (unsigned long)src.size();
        err = uncompress((unsigned char*)src.c_str(), &size, (const unsigned char*)zipbuf.c_str(), ziplen);
        if (err != Z_OK)
            return false;

        src.resize(size);
        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("%s", e.what());
        return false;
    }

	return false;
}

bool ZlibCpp::Decode(const char* zipbuf, unsigned long ziplen, char* src, unsigned long& srclen)
{
    if (ziplen == 0) return true;

    try
    {
        int err;
        unsigned long zipedsize = (unsigned long)ziplen;
        unsigned long size = srclen;
        err = uncompress((unsigned char*)src, &size, (const unsigned char*)zipbuf, zipedsize);
        if (err != Z_OK)
            return false;

        srclen = size;
        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("%s", e.what());
        return false;
    }

    return false;

}

size_t ZlibCpp::EncodeLen(unsigned long srclen)
{
    return (size_t)compressBound((unsigned long)srclen);
}

int gzcompress(Bytef *data, uLong ndata, Bytef *zdata, uLong *nzdata)
{
    z_stream c_stream;
    int err = 0;
    if (data && ndata > 0) {
        c_stream.zalloc = nullptr;
        c_stream.zfree = nullptr;
        c_stream.opaque = nullptr;
        //只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
        if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
            MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;

        c_stream.next_in = data;
        c_stream.avail_in = ndata;
        c_stream.next_out = zdata;
        c_stream.avail_out = *nzdata;
        while (c_stream.avail_in != 0 && c_stream.total_out < *nzdata) {
            if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
        }
        if (c_stream.avail_in != 0) return c_stream.avail_in;
        for (;;) {
            if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
            if (err != Z_OK) return -1;
        }
        if (deflateEnd(&c_stream) != Z_OK) return -1;
        *nzdata = c_stream.total_out;
        return 0;
    }
    return -1;
}

/* Uncompress gzip data */
/* zdata 数据 nzdata 原数据长度 data 解压后数据 ndata 解压后长度 */
int gzdecompress(Byte *zdata, uLong nzdata, Byte *data, uLong *ndata)
{
    int err = 0;
    z_stream d_stream = { 0 }; /* decompression stream */
    static char dummy_head[2] = {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = nullptr;
    d_stream.zfree = nullptr;
    d_stream.opaque = nullptr;
    d_stream.next_in = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = data;
    //只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
    if (inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK) return -1;
    //if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
    while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
        if (err != Z_OK) {
            if (err == Z_DATA_ERROR) {
                d_stream.next_in = (Bytef*)dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if ((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
                    return -1;
                }
            }
            else return -1;
        }
    }
    if (inflateEnd(&d_stream) != Z_OK) return -1;
    *ndata = d_stream.total_out;
    return 0;
}

bool ZlibCpp::EncodeGzip(const string& src, string& zipbuf)
{
    if (src.size() == 0) return true;

    try
    {
        Bytef *data = (Bytef*)src.c_str();
        uLong ndata = (unsigned long)src.length();

        if (src.size() > 65535)
        {
            zipbuf.resize(src.length());
        }
        else
            zipbuf.resize(65535);

        unsigned long nzdata = (unsigned long)zipbuf.length();
        Bytef* zdata = (Bytef*)zipbuf.c_str();

        int res = gzcompress(data, ndata, zdata, &nzdata);
        if (res == 0)
        {
            zipbuf.resize(nzdata);
            return true;
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("%s", e.what());
        return false;
    }
    return false;
}

bool ZlibCpp::EncodeGzipFile(const string& filename, string& zipbuf)
{
    string strSrc;
    if (!LoadBuf(filename, strSrc)) return false;
    return EncodeGzip(strSrc, zipbuf);
}

bool ZlibCpp::DecodeGzip(const string& zipbuf, string& src, unsigned long srclen)
{
    if (zipbuf.size() == 0) return true;

    try
    {
        unsigned long size = (unsigned long)zipbuf.size();
        src.clear();
        src.resize(srclen);
        unsigned long len = srclen;

        if (gzdecompress((Bytef *)zipbuf.c_str(), size, (Bytef *)src.c_str(), &len) == 0)
            return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("%s", e.what());
        return false;
    }

    return false;
}

bool ZlibCpp::LoadBuf(const string& filename, string& src)
{
    FILE* psLogFile = nullptr;
    psLogFile = fopen(filename.c_str(), "rb");
    if (psLogFile == nullptr)
        return false;
    size_t len = 0;
    fseek(psLogFile, 0, SEEK_END);
    len = ftell(psLogFile);
    src.resize(len + 1);
    fseek(psLogFile, 0, SEEK_SET);
    fread((char*)src.c_str(), len, 1, psLogFile);
    fclose(psLogFile);
    return true;
}