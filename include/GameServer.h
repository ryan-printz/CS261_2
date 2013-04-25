#pragma once

#include "IServer.h"
#include "ServerInfo.h"
#include "GameReplicationInfo.h"
#include "PlayerReplicationInfo.h"

#include <list>
#include <vector>

class UDPConnection;
class UDPConnectionManagerProcessThread;
class PlayerReplicationInfo;

// TODO: this needs to implement and inherit from IServer
// possibly needs common functionality implemented in a BaseServer class.
// needs to use UDP sockets/connections to connect to players.
typedef std::vector<PlayerReplicationInfo> PRIVector;

class GameServer : public IServer
{
public:
	GameServer() {}
	GameServer(UDPConnectionManagerProcessThread * gsThread);
	virtual ~GameServer();
	virtual void update();

	PRIVector & getPRIs();
	GameReplicationInfo & getGRI();
	ServerInfo & getInfo();

protected:
	int m_nextNetID;
	ServerInfo m_info;

	std::list<UDPConnection*> m_newConnections;
	UDPConnectionManagerProcessThread * m_gsThread;

	GameReplicationInfo m_GRI;
	PRIVector m_PRIs;
};
