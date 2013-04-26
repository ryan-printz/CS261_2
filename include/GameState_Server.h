#pragma once

#include "GameState_BasePlay.h"
#include "TCPConnection.h"
#include "ServerInfo.h"
#include "GameServer.h"
#include "GameReplicationInfo.h"
#include "PlayerReplicationInfo.h"
#include "MulticastSocket.h"
#include "NetObjectManager.h"
#include "ProtoConnection.h"

class GameState_Server : public GameState_BasePlay
{
public:
	GameState_Server(MulticastSocket * mc, ServerInfo & info, GameServer* gameServer);
	virtual ~GameState_Server();

	virtual void init();
	virtual void update();
	virtual void draw();
	virtual void unload();
	void destroyObject(short netID);
	virtual void onEnd() {};

protected:
	NetObjectManager m_netObjects;
	NetInstContainer m_Asteroids;
	MulticastSocket * m_master;
	GameServer * m_gameServer;
	float updateTimer;
};