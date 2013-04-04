#pragma once

#include "IServer.h"
#include "ServerInfo.h"

#include <list>

class UDPConnection;
class UDPConnectionManagerProcess;

// TODO: this needs to implement and inherit from IServer
// possibly needs common functionality implemented in a BaseServer class.
// needs to use UDP sockets/connections to connect to players.

class GameServer : public IServer
{
public:
	GameServer();
	virtual ~GameServer();

	virtual void update();

protected:
	ServerInfo m_info;

	std::list<UDPConnection*> m_newConnections;
	UDPConnectionManagerProcess * m_gsThread;
};
