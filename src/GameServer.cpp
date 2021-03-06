#include "GameServer.h"
#include "BaseNetMessage.h"
#include "ServerInfoNetMessage.h"
#include "ServerListNetMessage.h"
#include "ProtoConnection.h"
#include "ProtoConnectionManagerProcess.h"
#include "PlayerReplicationInfoNetMessage.h"
#include "GameReplicationInfoNetMessage.h"
#include "ClientInfoNetMessage.h"

#include <algorithm>

GameServer::GameServer(ProtoConnectionManagerProcessThread * gsthread)
	: m_gsThread(gsthread), m_nextNetID(1337)
{
	m_gsThread->m_manager->setAcceptCallback([this](ProtoConnection * accepted)
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

std::list<ProtoConnection*> & GameServer::getNewConnections()
{
	return m_newConnections;
}

bool GameServer::updatePlayerReplicationInfo(PlayerReplicationInfo & rPRI)
{
	// check if the player exists
	auto existingPRI = std::find_if(m_PRIs.begin(), m_PRIs.end(), 
		[rPRI](PlayerReplicationInfo const & pri)
		{
			return rPRI.m_netid == pri.m_netid;				
		});

	if( existingPRI != m_PRIs.end() )
	{
		*existingPRI = rPRI;
		return true;
	}

	return false;
}

void GameServer::addNewPlayer(ProtoConnection * connected, PlayerReplicationInfo & rPRI)
{
	int i = WSAGetLastError();
	// otherwise, new player. send them all the stuff they need.
	ClientInfo info;
	info.netID = m_nextNetID++;
	strcpy(info.name, rPRI.m_name);
	Packet clientInfo;
	i = WSAGetLastError();
	new (clientInfo.m_buffer) ClientInfoNetMessage(info);
	clientInfo.m_length = sizeof(ClientInfoNetMessage);
	i = WSAGetLastError();
	//std::cout << info.netID << std::endl;
	
	connected->send(clientInfo);
	i = WSAGetLastError();
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
	connected->send(gameInfo);
	i = WSAGetLastError();
	Packet playerInfo;
	for(auto pri = m_PRIs.begin(); pri != m_PRIs.end(); ++pri)
	{
		new (playerInfo.m_buffer) PlayerReplicationInfoNetMessage(*pri);
		playerInfo.m_length = sizeof(PlayerReplicationInfoNetMessage);
		connected->send(playerInfo);
		i = WSAGetLastError();
	}
}

void GameServer::sendGameInfo(int wave)
{
	m_GRI.m_PRICount = m_PRIs.size();
	m_GRI.m_wave = wave;

	Packet gri;
	new (gri.m_buffer) GameReplicationInfoNetMessage(m_GRI);
	gri.m_length = sizeof(GameReplicationInfoNetMessage);

	for( auto connected = m_newConnections.begin(); connected != m_newConnections.end(); ++connected )
	{
		(*connected)->send(gri);

		Packet pri;
		for( auto PRI = m_PRIs.begin(); PRI != m_PRIs.end(); ++PRI )
		{
			new (pri.m_buffer) PlayerReplicationInfoNetMessage(*PRI);
			pri.m_length = sizeof(PlayerReplicationInfoNetMessage);
			(*connected)->send(pri);
		}
	}
}

ObjectMsgList & GameServer::getObjectMsgs()
{
	return m_objectMsgs;
}

NinjaMsgList & GameServer::getNinjaMsgs()
{
	return m_ninjaMsgs;
}

void GameServer::update()
{
	m_gsThread->lock();
	//std::cout << "Update!" <<std::endl;
	// process new connections
	for( auto connected = m_newConnections.begin(); connected != m_newConnections.end();  )
	{
		(*connected)->m_lastRecv -= 1;
		if((*connected)->m_lastRecv <= 0)
		{
			Packet ninjaInfo;
			new (ninjaInfo.m_buffer) NinjaInfoCardMessage((*connected)->m_netID);
			ninjaInfo.m_length = sizeof(NinjaInfoCardMessage);
			for(int i = 0; i < m_PRIs.size(); ++i)
			{
				if(m_PRIs[i].m_netid == (*connected)->m_netID)
				{
					m_PRIs[i] = m_PRIs[m_PRIs.size()-1];
					m_PRIs.pop_back();
				}
			}
			//printf("Kill this connection\n");
			connected = m_newConnections.erase(connected);

			for( auto client = m_newConnections.begin(); client != m_newConnections.end(); ++client)
				if( client != connected )
					(*client)->send(ninjaInfo);
			continue;
		}

		Packet received;
		if( !(*connected)->pop_receivePacket(received) )
		{
			++connected;
			continue;
		}

		(*connected)->m_lastRecv = 500;
		BaseNetMessage * msg = reinterpret_cast<BaseNetMessage*>(received.m_buffer);

		if( msg->type() == PLAYER_REPLICATION_INFO )
		{
			int i = WSAGetLastError();
			auto pri = msg->as<PlayerReplicationInfoNetMessage>()->playerInfo();
			if( !updatePlayerReplicationInfo(pri) )
			{
				(*connected)->m_netID = m_nextNetID;
				i = WSAGetLastError();
				addNewPlayer(*connected, pri);
			}
										
			// new players update the player count
			m_info.currentPlayers = m_PRIs.size();
		}

		else if( msg->type() == OBJECT )
		{
			m_objectMsgs.push_back(*msg->as<ObjectNetMessage>());
			for( auto client = m_newConnections.begin(); client != m_newConnections.end(); ++client)
				if( client != connected )
					(*client)->send(received);
		}
		else if( msg->type() == NINJA_INFO )
		{
			m_ninjaMsgs.push_back(*msg->as<NinjaInfoCardMessage>());
			for( auto client = m_newConnections.begin(); client != m_newConnections.end(); ++client)
				if( client != connected )
					(*client)->send(received);
		}

		++connected;
	}


	m_gsThread->unlock();
}

int GameServer::getNextNetID()
{
	return m_nextNetID++;
}
