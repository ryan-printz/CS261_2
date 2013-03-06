// ---------------------------------------------------------------------------
// Project Name		:	Asteroid
// File Name		:	GameStateMgr.cpp
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	implementation of the game state manager
// History			:
// - 2008/02/08		:	- modified to be used like the old style game state
//						  manager
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#include "GameStateMgr.h"

#include "GameState_Play.h"
#include "GameState_Menu.h"
#include "GameState_Result.h"
#include "GameState_Platform.h"

void GameStateManager::init(IGameState * gameStateInit)
{
	// push the initial game state
	m_stateStack.emplace_back(gameStateInit);

	// reset the current, previous and next game
	m_gameStateNext = gameStateInit;
}

bool GameStateManager::quit()
{
	return (s32)m_stateStack.back() == GS_QUIT;
}

void GameStateManager::quit(bool quit)
{
	if( quit ) m_gameStateNext = (IGameState*)GS_QUIT;
}

bool GameStateManager::restart()
{
	return (s32)m_gameStateNext == GS_RESTART;
}

void GameStateManager::restart(bool restart)
{
	if( restart ) m_gameStateNext = (IGameState*)GS_RESTART;
}

bool GameStateManager::changeState()
{
	return m_stateStack.back() != m_gameStateNext;
}

void GameStateManager::nextState()
{
	m_stateStack.emplace_back(m_gameStateNext);
}

void GameStateManager::restartState()
{
	// make the next state the current state (instaed of GS_RESTART)
	m_gameStateNext = m_stateStack.back();
}

void GameStateManager::loadState()
{
	m_stateStack.back()->load();
}

void GameStateManager::initState()
{
	m_stateStack.back()->init();
}

void GameStateManager::updateState()
{
	m_stateStack.back()->update();
}

void GameStateManager::drawState()
{
	m_stateStack.back()->draw();
}

void GameStateManager::freeState()
{
	m_stateStack.back()->free();
}

void GameStateManager::unloadState()
{
	m_stateStack.back()->unload();
}

