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
#include <fstream>

// ---------------------------------------------------------------------------
// Defines

// ---------------------------------------------------------------------------
// Static variables

static s32 sCursor;
static std::fstream sConfig;

static char * sError = nullptr;

// ---------------------------------------------------------------------------

void GameStateNetworkMenuLoad(void)
{
	sConfig.open("config.txt");
}

// ---------------------------------------------------------------------------

void GameStateNetworkMenuInit(void)
{
	if( !sConfig.is_open() )
	{
		sError = "Missing Config.txt!";
		return;
	}

	// current selection
	sCursor = 0;
}

// ---------------------------------------------------------------------------

void GameStateNetworkMenuUpdate(void)
{
	if(AEInputCheckTriggered(DIK_UP))
		sCursor--;
	if(AEInputCheckTriggered(DIK_DOWN))
		sCursor++;
	
	sCursor = (sCursor < 0) ? 0 : ((sCursor > 1) ? 1 : sCursor);

	if(AEInputCheckTriggered(DIK_SPACE))
	{
		if (sCursor == 0)
			gGameStateNext = GS_PLAY;
		else
			gGameStateNext = GS_QUIT;
	}
}

// ---------------------------------------------------------------------------

void GameStateNetworkMenuDraw(void)
{
	if( sError )
	{
		AEGfxPrint(100, 100, 0xFF0000FF, sError);
		return;
	}

	AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID <>");
	AEGfxPrint(40, 60, 0xFFFFFFFF, "Start Game");
	AEGfxPrint(40, 90, 0xFFFFFFFF, "Quit");

	if (gAEFrameCounter & 0x0008)
		AEGfxPrint(10, 60 + 30 * sCursor, 0xFFFFFFFF, ">>");
}

// ---------------------------------------------------------------------------

void GameStateNetworkMenuFree(void)
{
}

// ---------------------------------------------------------------------------

void GameStateNetworkMenuUnload(void)
{
}

// ---------------------------------------------------------------------------
// Static function implementations


// ---------------------------------------------------------------------------
