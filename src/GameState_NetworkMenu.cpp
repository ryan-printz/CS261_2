
#include "main.h"
#include "GameState_NetworkMenu.h"
#include "GameState_Play.h"
#include <vector>

struct ServerInfo 
{
	// displayed info
	char * m_name;

	int m_players;
	int m_maxPlayers;

	// undisplayed info
	char * m_ip;
	short m_port;
};

// ---------------------------------------------------------------------------
struct ListGamesState : IGameState::State
{
	ListGamesState(IGameState * parent) : State(parent), m_cursor(0) {};
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
	SearchingState(IGameState * parent) : State(parent) {};
	void update();
	void draw();
};

struct ConnectingState : IGameState::State
{
	ConnectingState(IGameState * parent) : State(parent) {};
	void update();
	void draw();
};

/////////////

void ListGamesState::update()
{
	// list all the games that were listed on the server.

	if(AEInputCheckTriggered(DIK_UP))
		m_cursor--;
	if(AEInputCheckTriggered(DIK_DOWN))
		m_cursor++;
	
	m_cursor = (m_cursor < 0) ? 0 : ((m_cursor > m_games.size()) ? m_games.size() : m_cursor);

	if(AEInputCheckTriggered(DIK_SPACE))
		if( m_cursor == m_games.size() )
			popGameState();
		else if( m_cursor < m_games.size() )
			// move to the connecting state until the server accepts your connection attempt.
			m_parent->nextState(new ConnectingState(m_parent));
}

void ListGamesState::draw()
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID <>");
	
	int ypos = 60;
	for(auto game = m_games.begin(); game != m_games.end(); ++game)
	{
		AEGfxPrint(40, ypos, 0xFFFFFFFF, game->m_name);
		AEGfxPrint(100, ypos, 0xFFFFFFFF, game->m_ip);
		ypos += 30;
	}

	AEGfxPrint(40, ypos, 0xFFFFFFFF, "Back");

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

void SearchingState::update()
{
	// connect to the server and request active games

	// if the request times out, go to the failed state
	if( false )
		m_parent->nextState(new TimedOutState(m_parent));

	// if the request was successful, go to the listgamesstate;
	else if( true )
		m_parent->nextState(new ListGamesState(m_parent));
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
