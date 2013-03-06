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

// ---------------------------------------------------------------------------
// Static variables

// ---------------------------------------------------------------------------
// function implementations

void GameStateResultLoad(void)
{
	// nothing
}

// ---------------------------------------------------------------------------

void GameStateResultInit(void)
{
	// nothing
}

// ---------------------------------------------------------------------------

void GameStateResultUpdate(void)
{
	if ((gAEFrameCounter > 60) && (AEInputCheckTriggered(DIK_SPACE)))
		gGameStateNext = GS_MENU;
}

// ---------------------------------------------------------------------------

void GameStateResultDraw(void)
{
    AEGfxPrint(280, 260, 0xFFFFFFFF, "       GAME OVER       ");

	if (gAEFrameCounter > 60)
		AEGfxPrint(280, 300, 0xFFFFFFFF, "PRESS SPACE TO CONTINUE");
}

// ---------------------------------------------------------------------------

void GameStateResultFree(void)
{
	// nothing
}

// ---------------------------------------------------------------------------

void GameStateResultUnload(void)
{
	// nothing
}

// ---------------------------------------------------------------------------
// Static function implementations

// ---------------------------------------------------------------------------










