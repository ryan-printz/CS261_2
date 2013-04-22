
#include "main.h"
#include "GameState_NetworkMenu.h"
#include "GameState_Play.h"
#include "ServerInfo.h"
#include "TCPSocket.h"
#include "TCPConnection.h"
#include "BaseNetMessage.h"
#include "ServerInfoNetMessage.h"
#include "ServerListNetMessage.h"

#include <vector>
#include <fstream>
#include <iostream>

// ---------------------------------------------------------------------------
struct ListGamesState : IGameState::State
{
	ListGamesState(std::vector<ServerInfo> & games, IGameState * parent);
	void update();
	void draw();

	int m_cursor;
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
	ConnectingState(IGameState * parent) : State(parent) {};
	void update();
	void draw();
};

/////////////

ListGamesState::ListGamesState(std::vector<ServerInfo> & games, IGameState * parent) 
	: State(parent), m_cursor(0) 
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
	
	m_cursor = (m_cursor < 0) ? 0 : ((m_cursor >= m_games.size()) ? m_games.size() + 1 : m_cursor);

	if(AEInputCheckTriggered(DIK_SPACE))
		if( m_cursor == m_games.size() )
		{
			// do the auto join.
			// send a message to the master server
		}
		else if(m_cursor > m_games.size() )
			popGameState();
		else if( m_cursor < m_games.size() )
			// move to the connecting state until the server accepts your connection attempt.
			m_parent->nextState(new ConnectingState(m_parent));
}

void ListGamesState::draw()
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID SERVERS <>");
	int y = 50;
	for(int i = 0; i < m_games.size(); ++i)
		AEGfxPrint(40, y+=20, 0xFFFFFFFF, (s8*)m_games[i].info().c_str());

	AEGfxPrint(40, y, 0xFFFFFF, "Auto Join");
	AEGfxPrint(40, y+=20, 0xFFFFFFFF, "Back");

	if (gAEFrameCounter & 0x0008)
		AEGfxPrint(10, 60 + 30 * m_cursor, 0xFFFFFFFF, ">>");
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

	std::cout << WSAGetLastError() << std::endl;

	socket->connect(master);
	socket->setBlocking(false);

	m_master = new TCPConnection(socket, master);
}

SearchingState::~SearchingState()
{
	delete m_master;
}

void SearchingState::update()
{
	// connect to the server and request active games
	Packet received;
	
	// TODO: needs to update the connection.

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

		case AUTO_JOIN_RES:
			// connect to that server.
			break;
		};
	}

	// if the request times out, go to the failed state
	if( !m_master->connected() )
		m_parent->nextState(new TimedOutState(m_parent));

	// if the request was successful, go to the listgamesstate;
	else if( m_servers.size() == m_serverCount )
		m_parent->nextState(new ListGamesState(m_servers, m_parent));
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

void ConnectingState::update()
{
	// attempt to connect to the game server from the network lobby.

	if( false )
		m_parent->nextState(new TimedOutState(m_parent));

	else if( true ) 
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
