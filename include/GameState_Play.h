// ---------------------------------------------------------------------------
// Project Name		:	Asteroid Game
// File Name		:	GameState_Play.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	header file for the 'play' game state
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#pragma once

#include "GameState_BasePlay.h"

// ---------------------------------------------------------------------------

class GameState_Play : public GameState_BasePlay
{
public:
	GameState_Play();
	virtual ~GameState_Play();

	virtual void onEnd();
};
