#include "GameState_NetworkPlay.h"
#include "GameState_Result.h"
#include "BaseNetMessage.h"
#include "ServerInfoNetMessage.h"
#include "ServerListNetMessage.h"
#include "GameReplicationInfoNetMessage.h"
#include "PlayerReplicationInfoNetMessage.h"
#include "ClientInfoNetMessage.h"
#include "ObjectNetMessage.h"
#include "NetObjectManager.h"

GameState_NetworkPlay::GameState_NetworkPlay(GameReplicationInfo &gri, std::vector<PlayerReplicationInfo> &pris, ProtoConnection * gameServer, int netID)
	: m_GRI(gri), m_gameServer(gameServer), m_netID(netID), m_netObjects(&m_game), m_send(4), m_lastRecv(10.f)
{
	m_PRIs.swap(pris);
}

GameState_NetworkPlay::~GameState_NetworkPlay()
{}

void GameState_NetworkPlay::onEnd()
{
	m_gsm->nextState(new GameState_Result());
}

void GameState_NetworkPlay::init()
{
	GameState_BasePlay::init();

	// then create all the other players.
	for(int i = 0; i < m_PRIs.size(); ++i)
	{
		PlayerReplicationInfo & pri = m_PRIs[i];

		AEVec2 pos;

		pos.x = pri.m_x;
		pos.y = pri.m_y;

		m_game.gameObjInstCreate(TYPE_NET_SHIP, SHIP_SIZE, &pos, 0, pri.m_rotation, true);
	}
}

void GameState_NetworkPlay::unload()
{
	Packet disconnect;

	new (disconnect.m_buffer) BaseNetMessage(DISCONNECT);
	disconnect.m_length = sizeof(BaseNetMessage);

	m_gameServer->send(disconnect.m_buffer, disconnect.m_length);
}

void GameState_NetworkPlay::update()
{
	Packet received;

	if(m_gameServer)
	{
		m_gameServer->update(0.016);

		if( m_gameServer->pop_receivePacket(received) )
		{
			m_lastRecv = 10.f;
			BaseNetMessage * msg = reinterpret_cast<BaseNetMessage*>(received.m_buffer);

			switch( msg->type() )
			{
			case CLIENT_INFO:
				m_netID = msg->as<ClientInfoNetMessage>()->netID();
				break;

			case GAME_REPLICATION_INFO:
				m_GRI = msg->as<GameReplicationInfoNetMessage>()->gameInfo();
				break;

			case PLAYER_REPLICATION_INFO:
				updatePRI( msg->as<PlayerReplicationInfoNetMessage>()->playerInfo() );
				break;

			case OBJECT:
				push(*msg->as<ObjectNetMessage>());
				//m_netObjects.push(//updatePRI( msg->as<PlayerReplicationInfoNetMessage>()->playerInfo() );
				break;
			};
		}
	}

	updateInput();
	updatePhysics();
	updateObjects();

	for( NetInstContainer::iterator netObject = m_netObjects.begin(); netObject != m_netObjects.end(); ++netObject )
	{
		if( netObject->first == this->m_netID )
			continue;

		if ( netObject->second->pObject->type == TYPE_ASTEROID)
		{
			for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
			{
				asteroidCollide(netObject->second, &m_game.m_gameObjInsts[j]);
			}
		}

		else if ( netObject->second->pObject->type == TYPE_SHIP)
		{
			for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
			{
				if( playerCollide(netObject->second, &m_game.m_gameObjInsts[j]) )
				{
					// reduce the ship counter
					m_game.m_lives--;
				
					// if counter is less than 0, game over
					if (m_game.m_lives < 0)
					{
						m_game.m_gameTimer = 2.0;
						m_game.gameObjInstDestroy(m_game.m_localShip);
						m_game.m_localShip = 0;
					}

					break;
				}
			}
		}
		netObject->second;
	}

	updateMatrix();

	if(m_gameServer && !m_send)
	{
		Packet playerInfo;
	
		if(m_game.m_localShip)
		{
			new (playerInfo.m_buffer) ObjectNetMessage(m_netID, TYPE_NET_SHIP, FLAG_ACTIVE, 
														m_game.m_localShip->posCurr.x, 
														m_game.m_localShip->posCurr.y, 
														m_game.m_localShip->dirCurr, 
														m_game.m_localShip->velCurr.x, 
														m_game.m_localShip->velCurr.y);
			playerInfo.m_length = sizeof(ObjectNetMessage);

			m_gameServer->send(playerInfo);
		}
		m_send = 10;
	}
	m_lastRecv -= gAEFrameTime;
	m_send--;

	//Will only work once the server sends asteroid updates
	if(m_lastRecv <= 0 && m_gameServer)
	{
		printf("Kill this connection.\n");
		m_gameServer->disconnect();
		delete m_gameServer;
		m_gameServer = nullptr;
		onEnd();
	}
}

void GameState_NetworkPlay::draw()
{
	GameState_BasePlay::draw();

	int y = 50;
	int livePlayers = 0;
	PlayerReplicationInfo * lastPlayer = nullptr;

	AEGfxPrint(600, y, 0xFFDD0000, "SURVIVAL");

	for(int i = 0; i < m_PRIs.size(); ++i)
	{
		if( m_PRIs[i].m_lives < 0 )
			continue;

		livePlayers++;
		lastPlayer = &m_PRIs[i];

		char lives[2];

		AEGfxPrint(600, y+=20, 0xFFFFFFFF, itoa(m_PRIs[i].m_lives, lives, 10));
		AEGfxPrint(620, y, 0xFFCCCCCC, m_PRIs[i].m_name);
	}

	if( livePlayers == 1 && m_PRIs.size() > 1 )
	{
		char msg[256];
		sprintf(msg, "%s wins!", lastPlayer->m_name);
		AEGfxPrint(280, 280, 0xFF0000FF, msg);
	}	
}

void GameState_NetworkPlay::updatePRI(PlayerReplicationInfo& pri)
{
	for(int i = 0; i < m_PRIs.size(); ++i)
	{
		if(m_PRIs[i].m_netid == pri.m_netid)
		{
			m_PRIs[i].m_lives = pri.m_lives;
			m_PRIs[i].m_rotation = pri.m_rotation;
			m_PRIs[i].m_score = pri.m_score;
			m_PRIs[i].m_x = pri.m_x;
			m_PRIs[i].m_y = pri.m_y;
			memcpy(m_PRIs[i].m_name, pri.m_name, 12);
			return;
		}
	}
}

void GameState_NetworkPlay::push(ObjectNetMessage& obj)
{
	m_netObjects.update(obj.netId, obj.type, obj.flags, obj.x, obj.y, obj.z, obj.velx, obj.vely);
}