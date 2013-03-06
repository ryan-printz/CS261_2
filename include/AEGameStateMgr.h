// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEGameStateMgr.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2007/10/26
// Purpose			:	header file for the game state manager
// History			:
// - 2007/10/26		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AE_GAME_STATE_MGR_H
#define AE_GAME_STATE_MGR_H

// ---------------------------------------------------------------------------

#include "AEEngine.h"

// ---------------------------------------------------------------------------
// defines and enums

#define AE_GS_RESTART	0xFFFFFFFE
#define AE_GS_QUIT		0xFFFFFFFF

// ---------------------------------------------------------------------------
// externs

extern u32 gAEGameStateInit;
extern u32 gAEGameStateCurr;
extern u32 gAEGameStatePrev;
extern u32 gAEGameStateNext;

// ---------------------------------------------------------------------------

extern void (*AEGameStateLoad)();
extern void (*AEGameStateInit)();
extern void (*AEGameStateUpdate)();
extern void (*AEGameStateDraw)();
extern void (*AEGameStateFree)();
extern void (*AEGameStateUnload)();

// ---------------------------------------------------------------------------
// Function prototypes

// function to add new game state to the manager
void AEGameStateMgrAdd(u32 gameStateIdx, void (*pLoad)(), void (*pInit)(), void (*pUpdate)(), void (*pDraw)(), void (*pFree)(), void (*pUnload)());

// call this at the beginning and AFTER all game states are added to the manager
void AEGameStateMgrInit(u32 gameStateInit);

// update is used to set the function pointers
void AEGameStateMgrUpdate();

// ---------------------------------------------------------------------------

#endif // AE_GAME_STATE_MGR_H