#include "GameServer.h"
#include "BaseNetMessage.h"
#include "ServerInfoNetMessage.h"
#include "ServerListNetMessage.h"
#include "UDPConnection.h"
#include "UDPConnectionManagerProcess.h"
#include "GameReplicationInfo.h"

GameServer::GameServer(UDPConnectionManagerProcessThread * gsthread)
	: m_gsThread(gsthread)
{
	m_gsThread->m_manager->setAcceptCallback([this](UDPConnection * accepted)
	{
		m_newConnections.push_back(accepted);
	});
}

GameServer::~GameServer()
{}

void GameServer::update()
{
	m_gsThread->lock();

	// process new connections
	// servers are added to server list.
	// server list is sent to clients, then the client is disconnected.
	for( auto connected = m_newConnections.begin(); connected != m_newConnections.end(); ++connected )
	{
		Packet received;
		if( (*connected)->pop_receivePacket(received) )
		{
			printf("Hello\n");
			//if player replication info

			//if we have it, update it.

			//otherwise, new player. send them all the stuff they need.
		}
	}

	m_gsThread->unlock();
}
