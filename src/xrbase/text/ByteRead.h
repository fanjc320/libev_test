/*
 * ByteRead.h
 *
 *      Author: venture
 */

#pragma once

#include <string.h>
#include <stdlib.h>
#include <exception>
#include <stdexcept>
#include <vector>

class ByteRead
{
public:
	ByteRead();
	virtual ~ByteRead();

	void SetData(const char* pData, uint32_t nSize);
	const char* GetData() const;

	uint32_t Size() const;
	void ResetPos();
	uint32_t GetPos();

	void Clear();
	ByteRead& operator>>(float& val) { val = _Read<float>(); return *this; }
	ByteRead& operator>>(double& val) { val = _Read<double>(); return *this; }
	ByteRead& operator >> (bool& val) { val = (_Read<int8_t>() == 1); return *this; }
	ByteRead& operator>>(int8_t& val) { val = _Read<int8_t>(); return *this; }
	ByteRead& operator>>(uint8_t& val) { val = _Read<uint8_t>(); return *this; }
	ByteRead& operator>>(int16_t& val) { val = _Read<int16_t>(); return *this; }
	ByteRead& operator>>(uint16_t& val) { val = _Read<uint16_t>(); return *this; }
	ByteRead& operator>>(int32_t& val) { val = _Read<int32_t>(); return *this; }
	ByteRead& operator>>(uint32_t& val) { val = _Read<uint32_t>(); return *this; }
	ByteRead& operator>>(int64_t& val) { val = _Read<int64_t>(); return *this; }
	ByteRead& operator>>(uint64_t& val) { val = _Read<uint64_t>(); return *this; }
	ByteRead& operator>>(std::vector<float>& val) { _ReadVector<float>(val); return *this; }
	ByteRead& operator>>(std::vector<double>& val) { _ReadVector<double>(val); return *this; }
	ByteRead& operator>>(std::vector<int8_t>& val) { _ReadVector<int8_t>(val); return *this; }
	ByteRead& operator>>(std::vector<uint8_t>& val) { _ReadVector<uint8_t>(val); return *this; }
	ByteRead& operator>>(std::vector<int16_t>& val) { _ReadVector<int16_t>(val); return *this; }
	ByteRead& operator>>(std::vector<uint16_t>& val) { _ReadVector<uint16_t>(val); return *this; }
	ByteRead& operator>>(std::vector<int32_t>& val) { _ReadVector<int32_t>(val); return *this; }
	ByteRead& operator>>(std::vector<uint32_t>& val) { _ReadVector<uint32_t>(val); return *this; }
	ByteRead& operator>>(std::vector<int64_t>& val) { _ReadVector<int64_t>(val); return *this; }
	ByteRead& operator>>(std::vector<uint64_t>& val) { _ReadVector<uint64_t>(val); return *this; }

	template<typename T>
	ByteRead& operator>>(T& val) { val.deser(*this); return *this; }
	template<typename T>
	ByteRead& operator>>(std::vector<T>& val);
	ByteRead& operator>>(std::string& v) { _ReadStr(v); return *this; }

	template<typename T>
	ByteRead& ReadEnum(T& val) { val = (T)_Read<uint8>(); return *this; }

	void ReadBuf(std::string& strBuf, uint32_t nLen);
private:
	template<typename T>
	T& _Read();
	template<typename T>
	void _ReadVector(std::vector<T>& val);
	void _ReadStr(std::string& strVal);
	uint32_t _ReadLen();
protected:
	char*	 _pData = nullptr;
	uint32_t _nSize = 0;
	uint32_t _nPos = 0;
};

inline ByteRead::ByteRead()
	: _pData(nullptr)
	, _nSize(0)
	, _nPos(0)
{

}

inline void ByteRead::SetData(const char* pData, uint32_t nSize)
{
	_pData = const_cast<char*>(pData);
	_nSize = nSize;
	_nPos = 0;
}

inline ByteRead::~ByteRead()
{

}

inline void ByteRead::Clear()
{
	_pData = nullptr;
	_nSize = 0;
	_nPos = 0;
}

template<typename T>
inline T& ByteRead::_Read()
{
	if (!_pData)
		throw std::runtime_error("size error");

	if (_nPos + sizeof(T) > _nSize)
		throw std::runtime_error("size error");
	T& ret = *(T*)(_pData + _nPos);
	_nPos += sizeof(T);
	return ret;
}

inline void ByteRead::ReadBuf(std::string& strBuf, uint32_t nLen)
{
	if (!_pData)
		throw std::runtime_error("size error");

	if (_nPos + nLen > _nSize)
		throw std::runtime_error("size error");

	strBuf.clear();
	strBuf.append(_pData + _nPos, nLen);
	_nPos += nLen;
}

inline void ByteRead::_ReadStr(std::string& strVal)
{
	if (!_pData)
		throw std::runtime_error("size error");

	uint32_t nLen = _ReadLen();
	if (_nPos + nLen > _nSize)
		throw std::runtime_error("size error");

	strVal.clear();
	strVal.append(_pData + _nPos, nLen);
	_nPos += nLen;
}


uint32_t ByteRead::_ReadLen()
{
	uint8_t bt;
	uint32_t nLen;
	*this >> bt;
	nLen = bt & 0x7F;
	uint32_t nRx = 128;
	while (bt >= 128)
	{
		*this >> bt;
		nLen = (bt & 0x7F) * nRx + nLen;
		nRx = nRx * 128;
	}
	return nLen;
}

template<typename T>
inline ByteRead& ByteRead::operator>> (std::vector<T>& val)
{
	uint32 nLen = _ReadLen();
	val.clear();
	if (nLen > 0)
	{
		val.resize(nLen);
		for (uint32 i = 0; i < nLen; ++i)
		{
			val[i].deser(*this);
		}
	}
	return *this;
}

template<typename T>
inline void ByteRead::_ReadVector(std::vector<T>& val)
{
	uint32 nLen = _ReadLen();
	val.clear();
	if (nLen > 0)
	{
		val.resize(nLen);
		for (uint32 i = 0; i < nLen; ++i)
		{
			*this >> val[i];
		}
	}
}

inline uint32_t ByteRead::Size() const
{
	return _nSize;
}

inline const char* ByteRead::GetData() const
{
	return _pData;
}

inline void ByteRead::ResetPos()
{
	_nPos = 0;
}

inline uint32_t ByteRead::GetPos()
{
	return _nPos;
}
