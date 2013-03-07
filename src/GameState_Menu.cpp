// ---------------------------------------------------------------------------
// Project Name		:	Asteroid Game
// File Name		:	GameState_Menu.cpp
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/02/05
// Purpose			:	implementation of the 'menu' game state
// History			:
// - 2008/02/05		:	- initial implementation
// ---------------------------------------------------------------------------

#include "main.h"
#include "GameState_Menu.h"
#include "GameState_NetworkMenu.h"
#include "GameState_Play.h"

// ---------------------------------------------------------------------------

GameState_Menu::GameState_Menu()
{}

GameState_Menu::~GameState_Menu()
{}

void GameState_Menu::load(void)
{
	// nothing
}

// ---------------------------------------------------------------------------

void GameState_Menu::init(void)
{
	// current selection
	m_cursor = 0;
}

// ---------------------------------------------------------------------------

void GameState_Menu::update(void)
{
	if(AEInputCheckTriggered(DIK_UP))
		m_cursor--;
	if(AEInputCheckTriggered(DIK_DOWN))
		m_cursor++;
	
	m_cursor = (m_cursor < 0) ? 0 : ((m_cursor > 2) ? 2 : m_cursor);

	if(AEInputCheckTriggered(DIK_SPACE))
		switch(m_cursor)
		{
		case 0:
			m_gsm->nextState(new GameState_Play());
			break;

		case 1:
			m_gsm->nextState(new GameState_NetworkMenu());
			break;

		default:
			m_gsm->quit();
			break;
		};
}

// ---------------------------------------------------------------------------

void GameState_Menu::draw(void)
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID <>");
	AEGfxPrint(40, 60, 0xFFFFFFFF, "Start Game");
	AEGfxPrint(40, 60, 0xFFFFFFFF, "Network Menu");
	AEGfxPrint(40, 90, 0xFFFFFFFF, "Quit");

	if (gAEFrameCounter & 0x0008)
		AEGfxPrint(10, 60 + 30 * m_cursor, 0xFFFFFFFF, ">>");
}

// ---------------------------------------------------------------------------

void GameState_Menu::free(void)
{
}

// ---------------------------------------------------------------------------

void GameState_Menu::unload(void)
{
}
