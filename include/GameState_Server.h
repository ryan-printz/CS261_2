#pragma once

#include "GameState_BasePlay.h"
#include "TCPConnection.h"
#include "ServerInfo.h"
#include "GameServer.h"
#include "GameReplicationInfo.h"
#include "PlayerReplicationInfo.h"
#include "MulticastSocket.h"

class GameState_Server : public GameState_BasePlay
{
public:
	GameState_Server(MulticastSocket * mc, ServerInfo & info, GameServer* gameServer);
	virtual ~GameState_Server();

	virtual void init();
	virtual void update();
	virtual void draw();
	virtual void unload();

	virtual void onEnd() {};

protected:
	MulticastSocket * m_master;
	GameServer * m_gameServer;
};