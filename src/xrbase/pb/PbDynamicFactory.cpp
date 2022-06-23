/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "PbDynamicFactory.h"
#include <sys/types.h>
#include "platform/file_func.h"
#include <fstream>

void PbDynamicFactory::AddLinkPath(const std::string& proto_path)
{
    _sourceTree.MapPath("", proto_path);
    _vec_path.push_back(proto_path);
}

void PbDynamicFactory::Clear()
{
    _vec_path.clear();
    _init = false;
}

int PbDynamicFactory::Init(bool is_resume, const std::string& proto_path)
{
    AddLinkPath(proto_path);
    pb::compiler::Importer* imp = new pb::compiler::Importer(&_sourceTree, &_err_log);
    CHECK_LOG_ERROR(imp != nullptr);
    _importer.reset(imp);

    int ret = _LoadFileProto();
    CHECK_LOG_ERROR(ret == 0);

    _init = true;
    return 0;
}

void PbDynamicFactory::Uninit()
{}

int PbDynamicFactory::_LoadFileProto()
{
    for (auto itr = _vec_path.begin(); itr != _vec_path.end(); ++itr)
    {
        int ret = _LoadProtoPath(*itr);
        CHECK_LOG_ERROR(ret == 0);
    }

    return 0;
}

int PbDynamicFactory::_LoadProtoPath(const std::string& file_path)
{
    std::vector<std::string> vecFile;
    file::get_folder_file(file_path, vecFile, true, ".proto");
    for (auto file_name : vecFile)
    {
        const pb::FileDescriptor* pParsedFile = _importer->Import(file_name);
        if (pParsedFile == nullptr)
        {
            LOG_ERROR("protobuf runtime importer reloading %s. failed", file_name.c_str());
            return -1;
        }
        else
            LOG_INFO("protobuf runtime importer reloading %s. success", file_name.c_str());
    }

    return 0;
}

pb::compiler::Importer* PbDynamicFactory::Importer()
{
    if (!_init) return nullptr;
    return _importer.get();
}

pb::DynamicMessageFactory* PbDynamicFactory::Factory()
{
    if (!_init) return nullptr;
    return &_factory;
}