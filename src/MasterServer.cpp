#include "MasterServer.h"
#include "BaseNetMessage.h"
#include "ServerInfoNetMessage.h"
#include "ServerListNetMessage.h"
#include "TCPConnection.h"
#include "TCPConnectionManagerProcess.h"

#undef max(a, b)

#include <algorithm>

MasterServer::MasterServer(TCPConnectionManagerProcessThread * cmthread)
	: m_cmthread(cmthread), m_nextNetID(1337)
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

bool MasterServer::getLeastLoadServer(ServerInfo & server)
{
	short lowPlayers = std::numeric_limits<short>::max();
	auto leastLoadServer = m_servers.end();

	for( auto serverit = m_servers.begin(); serverit != m_servers.end(); ++serverit )
	{
		if( serverit->first.currentPlayers >= serverit->first.maxPlayers )
			continue;

		if( serverit->first.currentPlayers < lowPlayers )
		{
			lowPlayers = serverit->first.currentPlayers;
			leastLoadServer = serverit;
		}
	}

	if( leastLoadServer == m_servers.end() )
		return false;

	server = leastLoadServer->first;

	return true;
}

void MasterServer::pushServer(const ServerInfo & info, const NetAddress & address)
{
	auto server = std::find_if(m_servers.begin(), m_servers.end(), [address](const std::pair<ServerInfo, NetAddress> & pair)
		{
			return address.sin_addr.S_un.S_addr == pair.second.sin_addr.S_un.S_addr 
				&& address.sin_port == pair.second.sin_port
				&& address.sin_family == address.sin_family;
		});

	if ( server == m_servers.end() )
		m_servers.emplace_back(std::make_pair(info, address));
	else
		server->first = info;
}

void MasterServer::removeServer(const NetAddress & address)
{
	m_servers.remove_if([address](const std::pair<ServerInfo, NetAddress> & pair)
	{
		return address.sin_addr.S_un.S_addr == pair.second.sin_addr.S_un.S_addr 
			&& address.sin_port == pair.second.sin_port
			&& address.sin_family == address.sin_family;
	});
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
			
			if( msg->type() == SERVER_LIST_REQ )
			{
				Packet servers;

				new (servers.m_buffer) ServerListNetMessage(m_servers.size());
				servers.m_length = sizeof(ServerListNetMessage);

				(*connected)->push_sendPacket(servers);

				for(auto server = m_servers.begin(); server != m_servers.end(); ++server)
				{
					new (servers.m_buffer) ServerInfoNetMessage(server->first);
					servers.m_length = sizeof(ServerInfoNetMessage);

					(*connected)->push_sendPacket(servers);
				}

				ClientInfo info;
				info.netID = m_nextNetID++;

				m_clients.emplace_back(std::make_pair(info, *connected));
				connected = m_newConnections.erase(connected);
			}

			// if conencted is the end of the list after erase, incrementing it
			// causes an assert.
			if( connected == m_newConnections.end() )
				break;
		}
	}

	// update clients connected to the master server
	for( auto client = m_clients.begin(); client != m_clients.end(); ++client )
	{
		Packet received;

		while( client->second->pop_receivePacket(received) )
		{
			BaseNetMessage * msg = reinterpret_cast<BaseNetMessage*>(received.m_buffer);

			if ( msg->type() == AUTO_JOIN )
			{
				Packet server;
				ServerInfo serverInfo;

				if( getLeastLoadServer(serverInfo) )
				{
					new (server.m_buffer) ServerInfoNetMessage(serverInfo);
					server.m_length = sizeof(ServerInfoNetMessage);

					client->second->send(server);
				}
			}

			else if ( msg->type() == DISCONNECT )
			{
				client = m_clients.erase(client);
			}

			// erase can really screw up loops.
			if( client == m_clients.end() )
				break;
		}
	}

	m_cmthread->unlock();
}

const ServerVector & MasterServer::servers() const
{
	return m_servers;
}
