#pragma once

#include "dtype.h"
#include "server/SvrKindType.h"

class SvrKindID
{
public:
    virtual ~SvrKindID() { }

    void SetSvrKind(const std::string& szName, uint32_t dwArean, uint32_t id);
    void SetSvrKind(const std::string& szName, uint32_t dwType, uint32_t dwArean, uint32_t id);
	void SetSvrKind(uint32_t dwArean, uint32_t id);

    uint32_t GetSvrID() const { return _dwSvrID; }
    void SetSvrID(uint32_t val) { _dwSvrID = val; }

    const std::string& GetSvrName() const { return _szSvrName; }
    void SetSvrName(const std::string& val) { _szSvrName = val; }

    uint32_t GetSvrArean() const { return _dwSvrArean; }
    void SetSvrArean(uint32_t val) { _dwSvrArean = val; }

    uint32_t GetSvrType() const { return _dwSvrType; }
    void SetSvrType(uint32_t val) { _dwSvrType = val; }

    uint32_t GetKey() const { return _dwKey; }
    void SetKey(uint32_t val) { _dwKey = val; }

    std::string GetSvrKindKey();

protected:
    uint32_t        _dwSvrID = 1;
    uint32_t        _dwSvrArean = 1;
    uint32_t        _dwSvrType = 0;
    uint32_t        _dwKey = 0;
    std::string     _szSvrName;
};