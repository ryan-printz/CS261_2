// ---------------------------------------------------------------------------
// Project Name		:	Asteroid
// File Name		:	GameStateMgr.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2007/10/26
// Purpose			:	header file for the game state manager
// History			:
// - 2008/02/08		:   - updated to be used in conjuction with the build in
//						  game state manager in the Alpha Engine.
// - 2007/10/26		:	- initial implementation
// ---------------------------------------------------------------------------

#pragma once

// ---------------------------------------------------------------------------

#include "AEEngine.h"

// ---------------------------------------------------------------------------
// include the list of game states

#include "GameStateList.h"
#include "IGameState.h"

#include <vector>

class GameStateManager 
{
public:
	void init(IGameState * gameStateInit);

	// update is used to set the function pointers
	bool quit();
	void quit(bool quit);

	bool restart();
	void restart(bool restart);

	bool changeState();

	void nextState();
	void restartState();
	
	// state functions
	void loadState();
	void initState();
	void updateState();
	void drawState();
	void freeState();
	void unloadState();

private:
	std::vector <IGameState*> m_stateStack;
	IGameState *			  m_gameStateNext;
};
