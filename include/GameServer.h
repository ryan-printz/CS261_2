#pragma once

#include "IServer.h"
#include "ServerInfo.h"

#include <list>

class UDPConnection;
class UDPConnectionManagerProcessThread;
class PlayerReplicationInfo;

// TODO: this needs to implement and inherit from IServer
// possibly needs common functionality implemented in a BaseServer class.
// needs to use UDP sockets/connections to connect to players.

class GameServer : public IServer
{
public:
	GameServer() {}
	GameServer(UDPConnectionManagerProcessThread * gsThread);
	virtual ~GameServer();

	virtual void update();

protected:
	ServerInfo m_info;

	std::list<UDPConnection*> m_newConnections;
	UDPConnectionManagerProcessThread * m_gsThread;
};
