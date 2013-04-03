// ---------------------------------------------------------------------------
// Project Name		:	Asteroid Game
// File Name		:	GameState_Play.cpp
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	implementation of the 'play' game state
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#include "main.h"
#include "GameState_Play.h"
#include "GameState_Result.h"

GameState_Play::GameState_Play()
{};

GameState_Play::~GameState_Play()
{};

void GameState_Play::onEnd()
{
	m_gsm->nextState(new GameState_Result());
}