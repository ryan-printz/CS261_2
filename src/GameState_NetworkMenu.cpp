
#include "main.h"
#include "GameState_NetworkMenu.h"

// ---------------------------------------------------------------------------
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
	// current selection
	m_cursor = 0;
}

// ---------------------------------------------------------------------------

void GameState_NetworkMenu::update(void)
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
			//m_gsm->nextState(new GameState_NetworkPlay());
			break;

		default:
			m_gsm->popState();
			break;
		};
}

// ---------------------------------------------------------------------------

void GameState_NetworkMenu::draw(void)
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID <>");
	AEGfxPrint(40, 60, 0xFFFFFFFF, "Start Game");
	AEGfxPrint(40, 60, 0xFFFFFFFF, "Network Menu");
	AEGfxPrint(40, 90, 0xFFFFFFFF, "Quit");

	if (gAEFrameCounter & 0x0008)
		AEGfxPrint(10, 60 + 30 * m_cursor, 0xFFFFFFFF, ">>");
}

// ---------------------------------------------------------------------------

void GameState_NetworkMenu::free(void)
{
}

// ---------------------------------------------------------------------------

void GameState_NetworkMenu::unload(void)
{
}
