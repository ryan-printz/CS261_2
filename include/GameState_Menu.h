// ---------------------------------------------------------------------------
// Project Name		:	Asteroid Game
// File Name		:	GameState_Menu.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/02/05
// Purpose			:	header file for the 'menu' game state
// History			:
// - 2008/02/05		:	- initial implementation
// ---------------------------------------------------------------------------
#pragma once

#include "IGameState.h"

// ---------------------------------------------------------------------------

class GameState_Menu : public IGameState
{
public:
	GameState_Menu();
	virtual ~GameState_Menu();
	virtual void load(void);
	virtual void init(void);
	virtual void update(void);
	virtual void draw(void);
	virtual void free(void);
	virtual void unload(void);
};

// ---------------------------------------------------------------------------
