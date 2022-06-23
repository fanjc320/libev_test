/*
 *
 *      Author: venture
 */

#include <stdint.h>
#include <string>

#include "google/protobuf/message.h"
#include "google/protobuf/compiler/importer.h"
#include "google/protobuf/dynamic_message.h"
#include "log/LogMgr.h"

class PbDynamicError;

class PbDynamicError : public pb::compiler::MultiFileErrorCollector
{
    virtual void AddError(const std::string& filename, int line, int column, const std::string& message) override
    {
        LOG_ERROR("error in .proto file %s:%d:%d: %s", filename.c_str(), line + 1, column + 1, message.c_str());
    }
};

class PbDynamicFactory
{
public:
    int Init(bool is_resume, const std::string& proto_path);
    void Uninit();
    void AddLinkPath(const std::string& proto_path);
    void Clear();

    pb::compiler::Importer* Importer();
    pb::DynamicMessageFactory* Factory();

private:
    int _LoadFileProto();
    int _LoadProtoPath(const std::string& file_path);

    std::unique_ptr<pb::compiler::Importer> _importer;
    bool _init;

    pb::DynamicMessageFactory       _factory;
    PbDynamicError                  _err_log;
    pb::compiler::DiskSourceTree    _sourceTree;

    std::vector<std::string>        _vec_path;
};