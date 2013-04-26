#include "GameState_NetworkPlay.h"
#include "GameState_Result.h"
#include "BaseNetMessage.h"
#include "ServerInfoNetMessage.h"
#include "ServerListNetMessage.h"
#include "GameReplicationInfoNetMessage.h"
#include "PlayerReplicationInfoNetMessage.h"
#include "ClientInfoNetMessage.h"

GameState_NetworkPlay::GameState_NetworkPlay(GameReplicationInfo &gri, std::vector<PlayerReplicationInfo> &pris, ProtoConnection * gameServer)
	: m_GRI(gri), m_gameServer(gameServer)
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

void GameState_NetworkPlay::update()
{
	Packet received;

	m_gameServer->update(0.016);

	if( m_gameServer->pop_receivePacket(received) )
	{
		BaseNetMessage * msg = reinterpret_cast<BaseNetMessage*>(received.m_buffer);

		switch( msg->type() )
		{
		case CLIENT_INFO:
			//m_netID = msg->as<ClientInfoNetMessage>()->netID();
			break;

		case GAME_REPLICATION_INFO:
			m_GRI = msg->as<GameReplicationInfoNetMessage>()->gameInfo();
			break;

		case PLAYER_REPLICATION_INFO:
			m_PRIs.emplace_back( msg->as<PlayerReplicationInfoNetMessage>()->playerInfo() );
			break;
		};
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