#include "pch.h"
#include "ReconnMail.h"

BEGIN_EVENT1(ReconnMail, const MsgHead*)
EVENT(1, 1, &ReconnMail::OnTest)
END_EVENT1()

ReconnMail::ReconnMail() : SvrProgMq("xrmail", SVR_KIND_MAIL)
{

}


bool ReconnMail::OnTest(const MsgHead* pMsg)
{
	return true;
}
