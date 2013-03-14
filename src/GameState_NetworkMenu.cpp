
#include "main.h"
#include "GameState_NetworkMenu.h"

// ---------------------------------------------------------------------------

struct SearchingState : State
{
	SearchingState(IGameState * parent) : State(parent) {};

	void update()
	{
		// connect to the server and request active games
		// if the request times out, go to the failed state
		if( false )
			new TimedOutState(m_parent);

		// if the request was successful, go to the listgamesstate;
		else if( true )
			new ListGamesState(m_parent);
	}

	void draw()
	{
		AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID <>");

		if (gAEFrameCounter & 0x0008)
			AEGfxPrint(10, 50, 0xFFFFFFFF, "Searching for Games");
		else
			AEGfxPrint(10, 50, 0xFFFFFFFF, "Searching for Games...");
	}
};

struct ListGamesState : State
{
	ListGamesState(IGameState * parent) : State(parent) {};

	void update()
	{
		if(AEInputCheckTriggered(DIK_UP))
			m_cursor--;
		if(AEInputCheckTriggered(DIK_DOWN))
			m_cursor++;
	
		m_cursor = (m_cursor < 0) ? 0 : ((m_cursor > 1) ? 1 : m_cursor);

		if(AEInputCheckTriggered(DIK_SPACE))
			switch(m_cursor)
			{
			case 0:
				// connect to the the selected gamestate;
				new ConnectingState(/*connection info*/ m_parent);
				break;

			default:
				//m_gsm->popState();
				break;
			};
	}

	void draw()
	{
		AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID <>");
		AEGfxPrint(40, 60, 0xFFFFFFFF, "Join Game");
		AEGfxPrint(40, 90, 0xFFFFFFFF, "Back");

		if (gAEFrameCounter & 0x0008)
			AEGfxPrint(10, 60 + 30 * m_cursor, 0xFFFFFFFF, ">>");
	}

	int m_cursor;
};

struct ConnectingState : State
{
	ConnectingState(IGameState * parent) : State(parent) {};

	void update()
	{
		if( false )
			new TimedOutState(m_parent);

		else if( true ) 
		{
			//m_gsm->nextState(new GameSate_NetworkPlay(Connection));
		}
	}

	void draw()
	{
		AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID <>");

		if (gAEFrameCounter & 0x0008)
			AEGfxPrint(10, 50, 0xFFFFFFFF, "Connecting");
		else
			AEGfxPrint(10, 50, 0xFFFFFFFF, "Connecting...");
	}
};

struct TimedOutState : State
{
	TimedOutState(IGameState * parent) : State(parent) {};

	void update()
	{
	}

	void draw()
	{
		AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID <>");
		AEGfxPrint(10, 50, 0xFFFFFFFF, "Connection timed out!");
		AEGfxPrint(40, 70, 0xFFFFFFFF, "Back");

		if (gAEFrameCounter & 0x0008)
			AEGfxPrint(10, 70, 0xFFFFFFFF, ">>");
	}
};

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
