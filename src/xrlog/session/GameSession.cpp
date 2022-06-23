#include "pch.h"
#include "GameSession.h"

BEGIN_EVENT1(GameSession, const MsgHead*)
EVENT(1, 1, &GameSession::OnTest)
END_EVENT1()

GameSession::GameSession() : SvrProgMq("xrgame", SVR_KIND_GAME)
{

}


bool GameSession::OnTest(const MsgHead* pMsg)
{
	return true;
}
