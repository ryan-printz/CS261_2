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

// ---------------------------------------------------------------------------
// Defines

// ---------------------------------------------------------------------------
// Static variables

static s32 sCursor;

// ---------------------------------------------------------------------------

void GameStateMenuLoad(void)
{
	// nothing
}

// ---------------------------------------------------------------------------

void GameStateMenuInit(void)
{
	// current selection
	sCursor = 0;
}

// ---------------------------------------------------------------------------

void GameStateMenuUpdate(void)
{
	if(AEInputCheckTriggered(DIK_UP))
		sCursor--;
	if(AEInputCheckTriggered(DIK_DOWN))
		sCursor++;
	
	sCursor = (sCursor < 0) ? 0 : ((sCursor > 2) ? 2 : sCursor);

	if(AEInputCheckTriggered(DIK_SPACE))
	{
		if (sCursor == 0)
			gGameStateNext = GS_PLAY;
		else if( sCursor == 1 )
			gGameStateNext = GS_NETMENU;
		else
			gGameStateNext = GS_QUIT;
	}
}

// ---------------------------------------------------------------------------

void GameStateMenuDraw(void)
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID <>");
	AEGfxPrint(40, 60, 0xFFFFFFFF, "Start Game");
	AEGfxPrint(40, 60, 0xFFFFFFFF, "Network Menu");
	AEGfxPrint(40, 90, 0xFFFFFFFF, "Quit");

	if (gAEFrameCounter & 0x0008)
		AEGfxPrint(10, 60 + 30 * sCursor, 0xFFFFFFFF, ">>");
}

// ---------------------------------------------------------------------------

void GameStateMenuFree(void)
{
}

// ---------------------------------------------------------------------------

void GameStateMenuUnload(void)
{
}

// ---------------------------------------------------------------------------
// Static function implementations


// ---------------------------------------------------------------------------
