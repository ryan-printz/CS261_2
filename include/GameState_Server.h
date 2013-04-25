#pragma once

#include "GameState_BasePlay.h"
#include "TCPConnection.h"
#include "ServerInfo.h"
#include "GameServer.h"
#include "GameReplicationInfo.h"
#include "PlayerReplicationInfo.h"

class GameState_Server : public GameState_BasePlay
{
public:
	GameState_Server(TCPConnection * master, ServerInfo & info, GameServer* gameServer);
	virtual ~GameState_Server();

	virtual void init();
	virtual void update();
	virtual void draw();

	virtual void onEnd() {};

protected:
	TCPConnection * m_master;
	GameServer * m_gameServer;
};