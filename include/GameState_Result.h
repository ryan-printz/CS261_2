// ---------------------------------------------------------------------------
// Project Name		:	Asteroid Game
// File Name		:	GameState_Result.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/02/05
// Purpose			:	header file for the 'menu' game state
// History			:
// - 2008/02/05		:	- initial implementation
// ---------------------------------------------------------------------------
#pragma once

#include "IGameState.h"

// ---------------------------------------------------------------------------

class GameState_Result : public IGameState
{
public:
	GameState_Result();
	virtual ~GameState_Result();
	virtual void load(void);
	virtual void init(void);
	virtual void update(void);
	virtual void draw(void);
	virtual void free(void);
	virtual void unload(void);
};

// ---------------------------------------------------------------------------

