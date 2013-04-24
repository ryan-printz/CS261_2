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
	GameState_Server(TCPConnection * master, ServerInfo & info);
	virtual ~GameState_Server();

	virtual void update();
	virtual void draw();
protected:
	TCPConnection * m_master;
	ServerInfo m_info;
	GameServer * m_gameServer;
	GameReplicationInfo m_GRI;
	std::vector<PlayerReplicationInfo> m_PRIs;
};