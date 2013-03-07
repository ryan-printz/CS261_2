// ---------------------------------------------------------------------------
// Project Name		:	Asteroid Game
// File Name		:	GameState_Result.cpp
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/02/05
// Purpose			:	implementation of the 'result' state
// History			:
// - 2008/02/05		:	- initial implementation
// ---------------------------------------------------------------------------

#include "main.h"
#include "GameState_Result.h"
#include "GameState_Menu.h"

// ---------------------------------------------------------------------------
// Static variables

// ---------------------------------------------------------------------------
// function implementations

GameState_Result::GameState_Result()
{}

GameState_Result::~GameState_Result()
{}

void GameState_Result::load(void)
{
	// nothing
}

// ---------------------------------------------------------------------------

void GameState_Result::init(void)
{
	// nothing
}

// ---------------------------------------------------------------------------

void GameState_Result::update(void)
{
	// TODO: use popstate to return to the menu instead.
	if ((gAEFrameCounter > 60) && (AEInputCheckTriggered(DIK_SPACE)))
		m_gsm->nextState(new GameState_Menu());
}

// ---------------------------------------------------------------------------

void GameState_Result::draw(void)
{
    AEGfxPrint(280, 260, 0xFFFFFFFF, "       GAME OVER       ");

	if (gAEFrameCounter > 60)
		AEGfxPrint(280, 300, 0xFFFFFFFF, "PRESS SPACE TO CONTINUE");
}

// ---------------------------------------------------------------------------

void GameState_Result::free(void)
{
	// nothing
}

// ---------------------------------------------------------------------------

void GameState_Result::unload(void)
{
	// nothing
}









