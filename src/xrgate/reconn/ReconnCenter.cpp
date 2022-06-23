#include "pch.h"
#include "ReconnCenter.h"

BEGIN_EVENT1(ReconnCenter, const MsgHead*)
EVENT(1, 1, &ReconnCenter::OnTest)
END_EVENT1()

ReconnCenter::ReconnCenter() : SvrProgMq("xrcenter", SVR_KIND_CENTER)
{

}


bool ReconnCenter::OnTest(const MsgHead* pMsg)
{
	return true;
}
