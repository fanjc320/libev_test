#include "pch.h"
#include "ReconnHttp.h"

BEGIN_EVENT1(ReconnHttp, const MsgHead*)
EVENT(1, 1, &ReconnHttp::OnTest)
END_EVENT1()

ReconnHttp::ReconnHttp() : SvrProgMq("xrhttp", SVR_KIND_HTTP)
{

}


bool ReconnHttp::OnTest(const MsgHead* pMsg)
{
	return true;
}
