/*
 * ByteWrite.h
 *
 *      Author: venture
 */

#pragma once

#include <string.h>
#include <stdlib.h>
#include <exception>
#include <stdexcept>
#include <vector>
#include <string>

class ByteWrite
{
public:
	static const uint32_t DEFAULT_BYTE_SIZE = 8192;
	ByteWrite(uint32_t nMaxSize = DEFAULT_BYTE_SIZE);
	virtual ~ByteWrite();

	const char* GetData() const;
	const std::string& GetString() const;

	uint32_t GetSize() const;
	bool Resize(uint32_t nNewSize);

	void Clear();
	ByteWrite& operator<<(const float& val) { _Write<float>(val); return *this; }
	ByteWrite& operator<<(const double& val) { _Write<double>(val); return *this; }

	ByteWrite& operator<<(const bool& val) { _Write<int8_t>(val); return *this; }
	ByteWrite& operator<<(const int8_t& val) { _Write<int8_t>(val); return *this; }
	ByteWrite& operator<<(const uint8_t& val) { _Write<uint8_t>(val); return *this; }
	ByteWrite& operator<<(const int16_t& val) { _Write<int16_t>(val); return *this; }
	ByteWrite& operator<<(const uint16_t& val) { _Write<uint16_t>(val); return *this; }
	ByteWrite& operator<<(const int32_t& val) { _Write<int32_t>(val); return *this; }
	ByteWrite& operator<<(const uint32_t& val) { _Write<uint32_t>(val); return *this; }
	ByteWrite& operator<<(const int64_t& val) { _Write<int64_t>(val); return *this; }
	ByteWrite& operator<<(const uint64_t& val) { _Write<uint64_t>(val); return *this; }
	ByteWrite& operator<<(const std::vector<float>& val) { _WriteVector<float>(val); return *this; }
	ByteWrite& operator<<(const std::vector<double>& val) { _WriteVector<double>(val); return *this; }
	ByteWrite& operator<<(const std::vector<int8_t>& val) { _WriteVector<int8_t>(val); return *this; }
	ByteWrite& operator<<(const std::vector<uint8_t>& val) { _WriteVector<uint8_t>(val); return *this; }
	ByteWrite& operator<<(const std::vector<int16_t>& val) { _WriteVector<int16_t>(val); return *this; }
	ByteWrite& operator<<(const std::vector<uint16_t>& val) { _WriteVector<uint16_t>(val); return *this; }
	ByteWrite& operator<<(const std::vector<int32_t>& val) { _WriteVector<int32_t>(val); return *this; }
	ByteWrite& operator<<(const std::vector<uint32_t>& val) { _WriteVector<uint32_t>(val); return *this; }
	ByteWrite& operator<<(const std::vector<int64_t>& val) { _WriteVector<int64_t>(val); return *this; }
	ByteWrite& operator<<(const std::vector<uint64_t>& val) { _WriteVector<uint64_t>(val); return *this; }

	template<typename T>
	ByteWrite& operator<<(const T& val) { val.ser(*this); return *this; }
	template<typename T>
	ByteWrite& operator<<(const std::vector<T>& val);
	ByteWrite& operator<<(const std::string& v) { _WriteStr(v.c_str(), v.size()); return *this; }
	ByteWrite& operator<<(const char* sz) { _WriteStr(sz, strlen(sz)); return *this; }

	template<typename T>
	ByteWrite& WriteEnum(const T& val) { _Write<uint8>(val); return *this; }

	void WriteBuf(const char* szBuf, uint32_t nLen);
	void WriteBuf(uint32_t pos, const char* szBuf, uint32_t nLen);
private:

	template<typename T>
	void _Write(const T value);
	template<typename T>
	void _WriteVector(const std::vector<T>& val);
	void _WriteStr(const char* szValue, const uint32_t nLen);
	void _WriteLen(const uint32_t nLen);
protected:
	std::string _pData;
	uint32_t 	_nMaxSize = 0;
};

inline ByteWrite::ByteWrite(uint32_t nMaxSize)
	: _nMaxSize(nMaxSize)
{
	_pData.reserve(nMaxSize);
}

inline ByteWrite::~ByteWrite()
{
	_pData.clear();
	_nMaxSize = 0;
}

inline void ByteWrite::Clear()
{
	_pData.clear();
}

template<typename T>
void ByteWrite::_Write(const T value)
{
	uint32 nSize = _pData.size();
	if (nSize + sizeof(T) > _nMaxSize && !Resize(nSize + sizeof(T)))
	{
		throw std::runtime_error("size error");
	}

	_pData.append((const char*)(&value), sizeof(T));
}

inline bool ByteWrite::Resize(uint32_t nNewSize)
{
	if (nNewSize <= 0)
		return false;

	_pData.reserve(nNewSize);
	_nMaxSize = _pData.capacity();
	return true;
}

inline void ByteWrite::WriteBuf(const char* szBuf, uint32_t nLen)
{
	uint32_t nSize = _pData.size();
	if (nSize + nLen > _nMaxSize && !Resize(nSize + nLen))
	{
		throw std::runtime_error("size error");
	}

	_pData.append(szBuf, nLen);
}

inline void ByteWrite::WriteBuf(uint32_t pos, const char* szBuf, uint32_t nLen)
{
	uint32_t nSize = _pData.size();
	if (pos + nLen > nSize)
	{
		throw std::runtime_error("size error");
	}

	_pData.replace(pos, nLen, szBuf, nLen);
}

inline void ByteWrite::_WriteStr(const char* szValue,
	const uint32_t nLen)
{
	_WriteLen(nLen);
	uint32_t nSize = _pData.size();
	if (nSize + nLen > _nMaxSize && !Resize(nSize + nLen))
	{
		throw std::runtime_error("size error");
	}

	_pData.append(szValue, nLen);
}

void ByteWrite::_WriteLen(const uint32_t nLen)
{
	uint8_t bt[4] = { 0, 0, 0, 0 };
	uint8_t pos = 0;
	bt[0] = nLen & 0x7F;
	bt[1] = (nLen / 128) % 128;
	bt[2] = (nLen / 16384) % 128;;
	bt[3] = (nLen / 2097152) % 128;;
	if (bt[3] > 0) pos = 3;
	else if (bt[2] > 0) pos = 2;
	else if (bt[1] > 0) pos = 1;
	for (uint8_t i = 0; i < pos; ++i)
	{
		bt[i] = bt[i] | 0x80;
		*this << bt[i];
	}
	*this << bt[pos];
}

template<typename T>
inline ByteWrite& ByteWrite::operator<< (const std::vector<T>& val)
{
	uint32 nLen = val.size();
	_WriteLen(nLen);
	for (uint32 i = 0; i < nLen; ++i)
	{
		val[i].ser(*this);
	}
	return *this;
}

template<typename T>
inline void ByteWrite::_WriteVector(const std::vector<T>& val)
{
	uint32 nLen = val.size();
	_WriteLen(nLen);
	for (uint32 i = 0; i < nLen; ++i)
	{
		*this << val[i];
	}
}

inline uint32_t ByteWrite::GetSize() const
{
	return _pData.size();
}

inline const char* ByteWrite::GetData() const
{
	return _pData.c_str();
}

inline const std::string& ByteWrite::GetString() const
{
	return _pData;
}