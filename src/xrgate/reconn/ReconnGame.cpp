#include "pch.h"
#include "ReconnGame.h"

BEGIN_EVENT1(ReconnGame, const MsgHead*)
EVENT(1, 1, &ReconnGame::OnTest)
END_EVENT1()

ReconnGame::ReconnGame() : SvrProgMq("xrgame", SVR_KIND_GAME)
{

}

bool ReconnGame::OnTest(const MsgHead* pMsg)
{
	return true;
}
