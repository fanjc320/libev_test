/*
 *
 *      Author: venture
 */
#pragma once

#include <string>
using namespace std;
class ZlibCpp
{
public:
	ZlibCpp();
	~ZlibCpp();

    //zlib
	bool    Encode(const string& src, string& zipbuf);
    bool    Encode(const char* src, unsigned long srclen, char* zipbuf, unsigned long& ziplen);
    bool    EncodeFile(const string& filename, string& zipbuf);
    size_t  EncodeLen(unsigned long srclen);
	bool    Decode(const string& zipbuf, string& src, unsigned long srclen);
    bool    Decode(const char* zipbuf, unsigned long ziplen, char* src, unsigned long& srclen);

    //gzip
    bool    EncodeGzip(const string& src, string& zipbuf);
    bool    EncodeGzipFile(const string& filename, string& zipbuf);
    bool    DecodeGzip(const string& zipbuf, string& src, unsigned long srclen);

private:
    bool    LoadBuf(const string& filename, string& src);
};
