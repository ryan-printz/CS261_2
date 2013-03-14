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

struct State;

class GameState_NetworkMenu : public IGameState
{
public:
	GameState_NetworkMenu();
	virtual ~GameState_NetworkMenu();
	virtual void load(void);
	virtual void init(void);
	virtual void update(void);
	virtual void draw(void);
	virtual void free(void);
	virtual void unload(void);
};


