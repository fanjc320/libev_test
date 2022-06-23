#include "pch.h"
#include "ReconnLog.h"

BEGIN_EVENT1(ReconnLog, const MsgHead*)
EVENT(1, 1, &ReconnLog::OnTest)
END_EVENT1()

ReconnLog::ReconnLog() : SvrProgMq("xrlog", SVR_KIND_LOG)
{

}


bool ReconnLog::OnTest(const MsgHead* pMsg)
{
	return true;
}
