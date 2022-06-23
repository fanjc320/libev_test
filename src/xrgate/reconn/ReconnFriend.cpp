#include "pch.h"
#include "ReconnFriend.h"

BEGIN_EVENT1(ReconnFriend, const MsgHead*)
EVENT(1, 1, &ReconnFriend::OnTest)
END_EVENT1()

ReconnFriend::ReconnFriend() : SvrProgMq("xrfriend", SVR_KIND_FRIEND)
{

}


bool ReconnFriend::OnTest(const MsgHead* pMsg)
{
	return true;
}
