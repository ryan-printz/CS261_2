#include "MasterServer.h"
#include "BaseNetMessage.h"
#include "ServerInfoNetMessage.h"
#include "ServerListNetMessage.h"
#include "TCPConnection.h"
#include "TCPConnectionManagerProcess.h"

MasterServer::MasterServer(TCPConnectionManagerProcessThread * cmthread)
	: m_cmthread(cmthread)
{
	m_cmthread->m_manager->setAcceptCallback([this](TCPConnection * accepted)
	{
		m_newConnections.push_back(accepted);
	});
}

MasterServer::~MasterServer()
{
	delete m_cmthread;
}

void MasterServer::update()
{
	m_cmthread->lock();

	// process new connections
	// servers are added to server list.
	// server list is sent to clients, then the client is disconnected.
	for( auto connected = m_newConnections.begin(); connected != m_newConnections.end(); ++connected )
	{
		Packet received;
		if( (*connected)->pop_receivePacket(received) )
		{
			BaseNetMessage * msg = reinterpret_cast<BaseNetMessage*>(received.m_buffer);

			if( msg->type() == SERVER_INFO )
			{
				ServerInfo info = msg->as<ServerInfoNetMessage>()->info();

				m_servers.push_back(std::make_pair(info, *connected));
				connected = m_newConnections.erase(connected);

				printf("%s\t\t%i/%i - %s:%i", info.name, info.currentPlayers, info.maxPlayers, info.ip, info.port);
			}
			else if( msg->type() == SERVER_LIST_REQ )
			{
				Packet servers;

				new (servers.m_buffer) ServerListNetMessage(m_servers.size());
				(*connected)->push_sendPacket(servers);

				for(auto server = m_servers.begin(); server != m_servers.end(); ++server)
				{
					new (servers.m_buffer) ServerInfoNetMessage(server->first);
					(*connected)->push_sendPacket(servers);
				}

				connected = m_newConnections.erase(connected);
			}

			// if conencted is the end of the list after erase, incrementing it
			// causes an assert.
			if( connected == m_newConnections.end() )
				break;
		}
	}

	// update server info.
	for( auto server = m_servers.begin(); server != m_servers.end(); ++server )
	{
		Packet received;
		while( server->second->pop_receivePacket(received) )
		{
			// cast to message;
			BaseNetMessage * msg = reinterpret_cast<BaseNetMessage*>(received.m_buffer);

			switch( msg->type() )
			{
			case DISCONNECT:
				// server disconnected
				server = m_servers.erase(server);
				break;

			case SERVER_INFO:
				// serverinfo changed
				server->first = msg->as<ServerInfoNetMessage>()->info();
				break;
			}
		}
	}

	m_cmthread->unlock();
}

const ServerInfo & MasterServer::server(int index) const
{
	auto server = m_servers.begin();

	while( index-- )
		server++;

	return server->first;
}

int MasterServer::serverCount() const
{
	return m_servers.size();
}