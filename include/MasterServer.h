#pragma once

#include "ServerInfo.h"
#include "ClientInfo.h"
#include "ISocket.h"

#include <list>

class TCPConnection;
class TCPConnectionManagerProcessThread;

typedef std::list<std::pair<ServerInfo, NetAddress> > ServerVector;
typedef std::list<std::pair<ClientInfo, TCPConnection*> > ClientVector;

class MasterServer
{
public:
	MasterServer(TCPConnectionManagerProcessThread * cmthread);
	virtual ~MasterServer();

	virtual void update();

	void removeServer(const NetAddress & address);
	void pushServer(const ServerInfo & server, const NetAddress & address);

	const ServerVector & servers() const;

private:
	bool getLeastLoadServer(ServerInfo & server);

	TCPConnectionManagerProcessThread * m_cmthread;
	std::list<TCPConnection*> m_newConnections;
	ServerVector m_servers;
	ClientVector m_clients;
	char m_nextNetID;
};