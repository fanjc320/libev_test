/*
 *
 *      Author: venture
 */
#pragma once
#include <limits>
#include <stdint.h>
#include <functional>

#include "dtype.h"
#include "log/LogMgr.h"
#ifdef _MSC_VER
#include <windows.h>
#endif // _MSC_VER

typedef std::function<bool(const unsigned char* buf, int32_t len)> FUN_RING_BUF;
struct ring_buf
{
public:
    const static int32_t LEN_SIZE = sizeof(int32_t);
    const static int32_t MAX_VALID = INT32_MAX;

    void init(int32_t max_size, BOOL is_ring = TRUE, BOOL has_head_size = TRUE)
    {
        _head = 0;
        _tail = 0;
        _count = 0;
        _max_size = max_size;
        _is_ring = is_ring;
        _has_head_size = has_head_size;
    }

    BOOL is_ring() const
    {
        return _is_ring;
    }

    int32_t max_size() const
    {
        return _max_size;
    }

    bool empty() const
    {
        return _head == _tail;
    }

    bool full() const
    {
        return (_tail + 1) % _max_size == _head;
    }

    int32_t size() const
    {
        return (_tail - _head + _max_size) % _max_size;
    }
    void clear()
    {
        _head = 0;
        _tail = 0;
        _count = 0;
    }

    int32_t count() const
    {
        return _count;
    }

    // 1: 成功 0:失败
    int write(const unsigned char* buf, int32_t len)
    {
        int  ret = 0;
        int32_t  total = LEN_SIZE + len;
        int32_t  tail = _tail;
        int32_t  head = _head;
        ERROR_LOG_EXIT0(buf);
        if (!_is_ring && (tail + total) > _max_size)
        {
            ERROR_LOG_EXIT0(head > (total + 1) && head <= tail);
            memcpy(_content + tail, (void*) &MAX_VALID, LEN_SIZE);
            tail = 0;
        }
        ret = _write_chunk(head, tail, (const unsigned char*) (void*) &len, LEN_SIZE);
        ERROR_LOG_EXIT0(ret);
        ret = _write_chunk(head, tail, buf, len);
        ERROR_LOG_EXIT0(ret);

        _tail = tail;
        ++_count;

        return 1;
Exit0:
        return 0;
    }

    // 1: 成功 0: 失败 2: 空
    int read(unsigned char* buf, int32_t& len)
    {
        if (_count == 0) return 2;

        int ret = 0;
        int32_t head = _head;
        int32_t tail = _tail;
        int32_t total = LEN_SIZE;
        len = 0;
        ERROR_LOG_EXIT0(_count > 0);

        if (!_is_ring)
        {
            ret = _read_chunk_head(head, tail, len);
            ERROR_LOG_EXIT0(ret);
        }
        else
        {
            ret = _read_chunk(head, tail, (unsigned char*) (void*) &len, LEN_SIZE);
            ERROR_LOG_EXIT0(ret);
        }
        ret = _read_chunk(head, tail, buf, len);
        ERROR_LOG_EXIT0(ret);
        _head = head;
        --_count;

        return 1;
Exit0:
        return 0;
    }

    // 1: 成功 0: 失败 2: 空
    int for_each(FUN_RING_BUF func, int32_t& recv_times, int32_t times = MAX_VALID)
    {
        assert_retval(!_is_ring, 0);
        recv_times = 0;
        if (_count == 0) return 2;

        int ret = 0;
        int32_t count = _count;
        int32_t head = _head;
        int32_t tail = _tail;
        int32_t len = 0;
        bool fun_ret = true;
        ERROR_LOG_EXIT0(times > 0);
        for (int32_t i = 0; i < times && i < count; ++i)
        {
            tail = _tail;
            if (tail == head) return 3;
            len = 0;
            if (!_is_ring)
                ret = _read_chunk_head(head, tail, len);
            else
                ret = _read_chunk(head, tail, (unsigned char*) (void*) &len, LEN_SIZE);
            ERROR_LOG_EXIT0(ret);    
            const unsigned char* pBuf = _content + head;
            ret = _read_chunk(head, tail, nullptr, len);
            ERROR_LOG_EXIT0(ret);
            if (func != nullptr)
            {
                fun_ret = func(pBuf, (uint32_t)len);
                if (!fun_ret) return 4;
            }

            ++recv_times;
            _head = head;
            --_count;
        }
        return 1;
Exit0:
        return 0;
    }
private:
    int32_t _size(int32_t head, int32_t tail) const
    {
        return (tail - head + _max_size) % _max_size;
    }

    int32_t _unused_size(int32_t head, int32_t tail) const
    {
        return _max_size - _size(head, tail) - 1;
    }

    int _read_chunk(int32_t& head, int32_t tail, unsigned char* buf, int32_t len)
    {
        int ret = 0;

        ERROR_LOG_EXIT0(len <= _size(head, tail));

        if (head + len > _max_size)
        {
            int32_t rest = head + len - _max_size;
            if (buf)
            {
                memcpy(buf, _content + head, (int64_t)_max_size - head);
                memcpy(buf + ((int64_t)_max_size - head), _content, rest);
            }
            head = rest;
            return 2;
        }
        else
        {
            if (buf)
            {
                memcpy(buf, _content + head, len);
            }
            head += len;
        }

        return 1;
Exit0:
        return 0;
    }

    int _read_chunk_head(int32_t& head, int32_t tail, int32_t& len)
    {
        int ret = 0;
        int32_t total = LEN_SIZE;

        if (!_is_ring)
        {
            if ((head + total) > _max_size)
            {
                ERROR_LOG_EXIT0(tail >= total && head >= tail);
                head = 0;
                ret = _read_chunk(head, tail, (unsigned char*) (void*) &len, LEN_SIZE);
                ERROR_LOG_EXIT0(ret);
                return 2;
            }
            else
            {
                ret = _read_chunk(head, tail, (unsigned char*) (void*) &len, LEN_SIZE);
                ERROR_LOG_EXIT0(ret);
                if (len == MAX_VALID)
                {
                    ERROR_LOG_EXIT0(tail >= total && head >= tail);
                    head = 0;
                    ret = _read_chunk(head, tail, (unsigned char*) (void*) &len, LEN_SIZE);
                    ERROR_LOG_EXIT0(ret);
                    return 3;
                }
            }
        }
        else
        {
            ret = _read_chunk(head, tail, (unsigned char*) (void*) &len, LEN_SIZE);
            ERROR_LOG_EXIT0(ret);
        }
        return 1;
Exit0:
        return 0;
    }

    int _write_chunk(int32_t head, int32_t& tail, const unsigned char* buf, int32_t len)
    {
        ERROR_LOG_EXIT0(len <= _unused_size(head, tail));

        if (tail + len > _max_size)
        {
            int32_t rest = tail + len - _max_size;
            memcpy(_content + tail, buf, (int64_t)_max_size - tail);
            memcpy(_content, buf + ((int64_t) _max_size - tail), rest);
            tail = rest;
            return 2;
        }
        else
        {
            memcpy(_content + tail, buf, len);
            tail += len;
        }
        return 1;
Exit0:
        return 0;
    }

private:
    volatile int32_t _head;
    volatile int32_t _tail;
    volatile int32_t _count;
    int32_t          _max_size;
    BOOL             _is_ring;
    BOOL             _has_head_size;
    unsigned char    _content[0];//大的空间
};

static const int32_t RING_HEAD_SIZE = sizeof(ring_buf);
