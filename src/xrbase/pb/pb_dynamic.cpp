/*
 *
 *      Author: venture
 */
#include "pch.h"
#include "dtype.h"

#include "pb_dynamic.h"
#include "log/LogMgr.h"
#include "google/protobuf/message.h"

namespace pb_dynamic
{
	pb::Message* create_msg(const std::string& sProtoName)
	{
		pb::Message* pCmd = nullptr;
		const pb::Descriptor* descriptor = pb::DescriptorPool::generated_pool()->FindMessageTypeByName(sProtoName);
		if (descriptor == nullptr)
		{
			LOG_ERROR("FindMessageTypeByName %s Failed", sProtoName.c_str());
			return nullptr;
		}

		const pb::Message* prototype = pb::MessageFactory::generated_factory()->GetPrototype(descriptor);
		if (prototype == nullptr)
		{
			LOG_ERROR("GetPrototype %s Failed", sProtoName.c_str());
			return nullptr;
		}

		pCmd = prototype->New();
		if (pCmd == nullptr)
		{
			LOG_ERROR("New %s Failed", sProtoName.c_str());
			return nullptr;
		}

		return pCmd;
	}

	void delete_msg(pb::Message*& pMsg)
	{
		if (pMsg != nullptr)
		{
			delete pMsg;
			pMsg = nullptr;
		}
	}

	pb::Message* parse_from_string(const std::string& sProtoName, const std::string& sProtoData)
	{
		pb::Message* pCmd = create_msg(sProtoName);
		if (pCmd == nullptr)
		{
			LOG_ERROR("createMsg %s Failed", sProtoName.c_str());
			return nullptr;
		}

		if (!pCmd->ParseFromString(sProtoData))
		{
			LOG_ERROR("ParseFromString %s Failed", sProtoName.c_str());
			delete_msg(pCmd);
			return nullptr;
		}

		return pCmd;
	}

	pb::Message* parse_from_array(const std::string& sProtoName, const char* szProtoData, const uint32_t iLen)
	{
		pb::Message* pCmd = create_msg(sProtoName);
		if (pCmd == nullptr)
		{
			LOG_ERROR("createMsg %s Failed", sProtoName.c_str());
			return nullptr;
		}

		if (!pCmd->ParseFromArray(szProtoData, iLen))
		{
			LOG_ERROR("ParseFromArray %s Failed", sProtoName.c_str());
			delete_msg(pCmd);
			return nullptr;
		}

		return pCmd;
	}
}