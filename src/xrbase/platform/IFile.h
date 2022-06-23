/*
 *
 *      Author: venture
 */
#pragma once
#include <stdint.h>
#include <time.h>

namespace file
{
    class IFile
    {
    public:
        virtual size_t Read(void* pData, size_t dwLen) = 0;
        virtual bool  Seek(size_t nOffset, size_t nOrigin) = 0;
        virtual bool Eof(void) = 0;
        virtual size_t Size(void) = 0;
    };

    IFile* open_file(const char* pcszFileName, const char* pcszMode);
    bool close_file(IFile* pFile);

    time_t get_file_create_time(const char* pcszFileName);
    time_t get_file_modify_time(const char* pcszFileName);
}
