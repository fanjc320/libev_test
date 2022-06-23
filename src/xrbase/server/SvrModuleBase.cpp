#include "pch.h"
#include <typeinfo>
#include "SvrModuleBase.h"
#include "string/str_format.h"

SvrModuleBase::SvrModuleBase(const std::string& moduleid) :_id(moduleid)
{

}

const std::string& SvrModuleBase::GetModuleID() 
{ 
    if (_id.empty())
    {
		_id = typeid(*this).name();
        _id = str_replace(_id, "class ", "");
    }
    return _id;
}