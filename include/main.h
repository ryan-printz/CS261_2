// ---------------------------------------------------------------------------
// Project Name		:	Asteroid
// File Name		:	main.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/02/05
// Purpose			:	main header file for the asteroid game
// History			:
// - 2007/02/05		:	- initial implementation
// ---------------------------------------------------------------------------

#pragma once

// ---------------------------------------------------------------------------

#define USE_CUSTOM_GAME_STATE_MGR	1

// ---------------------------------------------------------------------------
#if USE_CUSTOM_GAME_STATE_MGR
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// includes

#include "AEEngine.h"

// game state manager
#include "GameStateMgr.h"

// ---------------------------------------------------------------------------
#else // USE_CUSTOM_GAME_STATE_MGR
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// includes

#include "AEEngine.h"

// ---------------------------------------------------------------------------
// game state list

enum
{
	// list of all game states
	GS_PLAY    = 0, 
	GS_MENU, 
	GS_RESULT, 
	
	// special game state. Do not change
	GS_RESTART = AE_GS_RESTART,
	GS_QUIT    = AE_GS_QUIT
};

// ---------------------------------------------------------------------------
// define the state related variables and functions to use the one 
// from the Alpha Engine

#define gGameStateInit		gAEGameStateInit
#define gGameStateCurr		gAEGameStateCurr
#define gGameStatePrev		gAEGameStatePrev
#define gGameStateNext		gAEGameStateNext

#define GameStateLoad		AEGameStateLoad	
#define GameStateInit		AEGameStateInit	
#define GameStateUpdate		AEGameStateUpdate	
#define GameStateDraw		AEGameStateDraw	
#define GameStateFree		AEGameStateFree	
#define GameStateUnload		AEGameStateUnload	

#define GameStateMgrInit	AEGameStateMgrInit
#define GameStateMgrUpdate	AEGameStateMgrUpdate

// ---------------------------------------------------------------------------
#endif // USE_CUSTOM_GAME_STATE_MGR
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------

#endif // MAIN_H











