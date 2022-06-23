#include "pch.h"
#include "GateSession.h"

BEGIN_EVENT1(GateSession, const MsgHead*)
EVENT(1, 1, &GateSession::OnTest)
END_EVENT1()

GateSession::GateSession() : SvrProgMq("xrgate", SVR_KIND_GATE)
{

}


bool GateSession::OnTest(const MsgHead* pMsg)
{
	return true;
}
