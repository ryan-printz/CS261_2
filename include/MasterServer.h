#pragma once

#include "ServerInfo.h"
#include <list>

class TCPConnection;
class TCPConnectionManagerProcessThread;

class MasterServer
{
public:
	MasterServer(TCPConnectionManagerProcessThread * cmthread);
	virtual ~MasterServer();

	virtual void update();

	const ServerInfo & server(int index) const;
	int serverCount() const;

private:
	std::list<TCPConnection*> m_newConnections;
	std::list<std::pair<ServerInfo, TCPConnection*> > m_servers;

	TCPConnectionManagerProcessThread * m_cmthread;
};