
#include "main.h"
#include "GameState_NetworkMenu.h"
#include "GameState_Play.h"
#include "ServerInfo.h"
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "TCPConnection.h"
#include "UDPConnection.h"
#include "BaseNetMessage.h"
#include "ServerInfoNetMessage.h"
#include "ServerListNetMessage.h"
#include "GameReplicationInfoNetMessage.h"
#include "PlayerReplicationInfoNetMessage.h"

#include <vector>
#include <fstream>
#include <iostream>

// ---------------------------------------------------------------------------
struct ListGamesState : IGameState::State
{
	ListGamesState(std::vector<ServerInfo> & games, TCPConnection * master, IGameState * parent);
	void update();
	void draw();

	int m_cursor;
	TCPConnection * m_master;
	std::vector<ServerInfo> m_games;
};

struct TimedOutState : IGameState::State
{
	TimedOutState(IGameState * parent) : State(parent) {};
	void update();
	void draw();
};

struct SearchingState : IGameState::State
{
	SearchingState(IGameState * parent);
	~SearchingState();

	void update();
	void draw();

	std::vector<ServerInfo> m_servers;
	int m_serverCount;
	TCPConnection * m_master;
};

struct ConnectingState : IGameState::State
{
	ConnectingState(ServerInfo & server, IGameState * parent);

	void update();
	void draw();

	UDPConnection * m_game;
	GameReplicationInfo m_GRI;
	std::vector<PlayerReplicationInfo> m_PRIs;
};

struct AutoJoinState : IGameState::State
{
	AutoJoinState(TCPConnection * master, IGameState * parent);
	~AutoJoinState();

	void update();
	void draw() {};

	TCPConnection * m_master;
};

//////////////

AutoJoinState::AutoJoinState(TCPConnection * master, IGameState * parent)
	: State(parent), m_master(master)
{
	Packet autoJoinRequest;

	new (autoJoinRequest.m_buffer) BaseNetMessage(AUTO_JOIN);
	autoJoinRequest.m_length = sizeof(BaseNetMessage);

	m_master->send(autoJoinRequest);
}

void AutoJoinState::update()
{
	Packet received;

	m_master->update(0.016);

	if( m_master->pop_receivePacket(received) )
	{
		BaseNetMessage * msg = reinterpret_cast<BaseNetMessage*>(received.m_buffer);

		switch( msg->type() )
		{
		case AUTO_JOIN_RES:
		case SERVER_INFO:
			ServerInfo serverInfo = msg->as<ServerInfoNetMessage>()->info();
			m_parent->nextState(new ConnectingState(serverInfo, m_parent));
			break;
		};
	}

	if( !m_master->connected() )
		m_parent->nextState(new TimedOutState(m_parent));
}

/////////////

ListGamesState::ListGamesState(std::vector<ServerInfo> & games, TCPConnection * master, IGameState * parent) 
	: State(parent), m_master(master), m_cursor(0) 
{
	m_games.swap(games);
}

void ListGamesState::update()
{
	// list all the games that were listed on the server.

	if(AEInputCheckTriggered(DIK_UP))
		m_cursor--;
	if(AEInputCheckTriggered(DIK_DOWN))
		m_cursor++;
	
	m_cursor = (m_cursor < 0) ? 0 : ((m_cursor > m_games.size() + 1) ? m_games.size() + 1 : m_cursor);

	if(AEInputCheckTriggered(DIK_SPACE))
		if( m_cursor == m_games.size() )
			m_parent->nextState(new AutoJoinState(m_master, m_parent));

		else if(m_cursor > m_games.size() )
			popGameState();

		else if( m_cursor < m_games.size() )
			// move to the connecting state until the server accepts your connection attempt.
			m_parent->nextState(new ConnectingState(m_games[m_cursor], m_parent));
}

void ListGamesState::draw()
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID SERVERS <>");
	int y = 50;
	for(int i = 0; i < m_games.size(); ++i)
		AEGfxPrint(40, y+=20, 0xFFFFFFFF, (s8*)m_games[i].info().c_str());

	AEGfxPrint(40, y+=20, 0xFFFFFFFF, "Auto Join");
	AEGfxPrint(40, y+=20, 0xFFFFFFFF, "Back");

	if (gAEFrameCounter & 0x0008)
		AEGfxPrint(10, 70 + 20 * m_cursor, 0xFFFFFFFF, ">>");
}

///////////////////

void TimedOutState::update()
{
	if(AEInputCheckTriggered(DIK_SPACE))
		popGameState();
}

void TimedOutState::draw()
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID <>");
	AEGfxPrint(10, 50, 0xFFFFFFFF, "Connection timed out!");
	AEGfxPrint(40, 70, 0xFFFFFFFF, "Back");

	if (gAEFrameCounter & 0x0008)
		AEGfxPrint(10, 70, 0xFFFFFFFF, ">>");
}

///////////////////////////////

SearchingState::SearchingState(IGameState * parent) 
	: State(parent) 
{
	std::ifstream config("config.txt");

	if( !config.is_open() )
		std::cout << "failed to open config file" << std::endl;

	char ip[16];
	short port;

	config.getline(ip, sizeof(ip));
	config >> port;
	config.close();

	NetAddress master(ip, port);

	auto socket = new TCPSocket();

	socket->initialize(master);
	socket->connect(master);
	socket->setBlocking(false);

	m_master = new TCPConnection(socket, master);

	Packet request;
	new (request.m_buffer) ServerListReqNetMessage();
	request.m_length = sizeof(BaseNetMessage);

	m_master->send(request);
}

SearchingState::~SearchingState()
{
	delete m_master;
}

void SearchingState::update()
{
	// connect to the server and request active games
	Packet received;
	
	m_master->update(0.016f);

	if( m_master->pop_receivePacket(received) )
	{
		BaseNetMessage * msg = reinterpret_cast<BaseNetMessage*>(received.m_buffer);

		switch( msg->type() )
		{
		case SERVER_LIST:
			m_serverCount = msg->as<ServerListNetMessage>()->count();
			m_servers.reserve(m_serverCount);
			break;

		case SERVER_INFO:
			m_servers.push_back(msg->as<ServerInfoNetMessage>()->info());
			break;
		};
	}

	// if the request times out, go to the failed state
	if( !m_master->connected() )
		m_parent->nextState(new TimedOutState(m_parent));

	// if the request was successful, go to the listgamesstate;
	else if( m_servers.size() == m_serverCount )
		m_parent->nextState(new ListGamesState(m_servers, m_master, m_parent));
}

void SearchingState::draw()
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID <>");

	if (gAEFrameCounter & 0x0008)
		AEGfxPrint(10, 50, 0xFFFFFFFF, "Searching for Games");
	else
		AEGfxPrint(10, 50, 0xFFFFFFFF, "Searching for Games...");
}

///////////////////////////////////

ConnectingState::ConnectingState(ServerInfo & server, IGameState * parent)
	: State(parent)
{
	std::cout << "connecting to server: " << server.name << " (" << server.ip << ":" << server.port << ")" << std::endl;

	NetAddress gameServerAddress(server.ip, server.port);

	auto socket = new UDPSocket();

	socket->initialize(gameServerAddress);
	socket->connect(gameServerAddress);
	socket->setBlocking(false);

	m_game = new UDPConnection(socket, gameServerAddress);

	Packet playerInfo;
	PlayerReplicationInfo pri;

	new (playerInfo.m_buffer) PlayerReplicationInfoNetMessage(pri);
	playerInfo.m_length = sizeof(PlayerReplicationInfoNetMessage);

	m_game->send(playerInfo);
	int i = WSAGetLastError();
	printf("%i\n",i);
}

void ConnectingState::update()
{
	Packet received;

	m_game->update(0.016);

	if( m_game->pop_receivePacket(received) )
	{
		BaseNetMessage * msg = reinterpret_cast<BaseNetMessage*>(received.m_buffer);

		switch( msg->type() )
		{
		case GAME_REPLICATION_INFO:
			m_GRI = msg->as<GameReplicationInfoNetMessage>()->gameInfo();
			break;

		case PLAYER_REPLICATION_INFO:
			m_PRIs.emplace_back( msg->as<PlayerReplicationInfoNetMessage>()->playerInfo() );
			break;
		};
	}

	if( !m_game->connected() )
		m_parent->nextState(new TimedOutState(m_parent));

	else if( m_GRI.m_inProgress && m_GRI.m_PRICount == m_PRIs.size() ) 
		nextGameState(new GameState_Play());
}

void ConnectingState::draw()
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID <>");

	if (gAEFrameCounter & 0x0008)
		AEGfxPrint(10, 50, 0xFFFFFFFF, "Connecting");
	else
		AEGfxPrint(10, 50, 0xFFFFFFFF, "Connecting...");
}

////////////////////////////////////

GameState_NetworkMenu::GameState_NetworkMenu()
{}

GameState_NetworkMenu::~GameState_NetworkMenu()
{}

void GameState_NetworkMenu::load(void)
{
	// nothing
}

// ---------------------------------------------------------------------------

void GameState_NetworkMenu::init(void)
{
	m_state = new SearchingState(this);
}

// ---------------------------------------------------------------------------

void GameState_NetworkMenu::update(void)
{
	m_state->update();
}

// ---------------------------------------------------------------------------

void GameState_NetworkMenu::draw(void)
{
	m_state->draw();
}

// ---------------------------------------------------------------------------

void GameState_NetworkMenu::free(void)
{
	delete m_state;
}

// ---------------------------------------------------------------------------

void GameState_NetworkMenu::unload(void)
{
}
