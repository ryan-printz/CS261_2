#include "GameServer.h"
#include "BaseNetMessage.h"
#include "ServerInfoNetMessage.h"
#include "ServerListNetMessage.h"
#include "UDPConnection.h"
#include "UDPConnectionManagerProcess.h"
#include "PlayerReplicationInfoNetMessage.h"
#include "GameReplicationInfoNetMessage.h"
#include "ClientInfoNetMessage.h"

#include <algorithm>

GameServer::GameServer(UDPConnectionManagerProcessThread * gsthread)
	: m_gsThread(gsthread), m_nextNetID(1337)
{
	m_gsThread->m_manager->setAcceptCallback([this](UDPConnection * accepted)
	{
		m_newConnections.push_back(accepted);
	});
}

GameServer::~GameServer()
{}

PRIVector & GameServer::getPRIs()
{
	return m_PRIs;
}

GameReplicationInfo & GameServer::getGRI()
{
	return m_GRI;
}

ServerInfo & GameServer::getInfo()
{
	return m_info;
}

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
			BaseNetMessage * msg = reinterpret_cast<BaseNetMessage*>(received.m_buffer);

			if( msg->type() == PLAYER_REPLICATION_INFO )
			{
				//if we have it, update it.
				auto rPRI = msg->as<PlayerReplicationInfoNetMessage>()->playerInfo();

				auto existingPRI = std::find_if(m_PRIs.begin(), m_PRIs.end(), 
					[rPRI](PlayerReplicationInfo const & pri)
					{
						return rPRI.m_netid == pri.m_netid;				
					});

				if( existingPRI != m_PRIs.end() )
				{
					*existingPRI = rPRI;
				}

				//otherwise, new player. send them all the stuff they need.
				else
				{
					ClientInfo info;
					info.netID = m_nextNetID++;

					Packet clientInfo;
					new (clientInfo.m_buffer) ClientInfoNetMessage(info);
					clientInfo.m_length = sizeof(ClientInfoNetMessage);

					(*connected)->send(clientInfo);

					rPRI.m_lives = 3;
					rPRI.m_rotation = 0;
					rPRI.m_score = 0;
					rPRI.m_x = 400;
					rPRI.m_y = 300;
					rPRI.m_netid = info.netID;

					m_PRIs.emplace_back(rPRI);

					m_GRI.m_PRICount = m_PRIs.size();

					Packet gameInfo;
					new (gameInfo.m_buffer) GameReplicationInfoNetMessage(m_GRI);
					gameInfo.m_length = sizeof(GameReplicationInfoNetMessage);
					(*connected)->send(gameInfo);

					Packet playerInfo;
					for(auto pri = m_PRIs.begin(); pri != m_PRIs.end(); ++pri)
					{
						new (playerInfo.m_buffer) PlayerReplicationInfoNetMessage(*pri);
						playerInfo.m_length = sizeof(PlayerReplicationInfoNetMessage);
						(*connected)->send(playerInfo);
					}
				}
			}
		}
	}

	m_gsThread->unlock();
}
