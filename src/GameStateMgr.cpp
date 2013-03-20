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
#include "IGameState.h"

#include "GameState_Play.h"
#include "GameState_Menu.h"
#include "GameState_Result.h"
#include "GameState_Platform.h"

void GameStateManager::init(IGameState * gameStateInit)
{
	// push the initial game state
	m_gameStateInit = m_gameStateNext
					= m_gameStatePrev
					= m_gameStateCurr = gameStateInit;

	gameStateInit->setGameStateManager(this);
}

void GameStateManager::destroyState(IGameState * state)
{
	if( state != (IGameState*)GS_QUIT && state != (IGameState*)GS_RESTART )
		delete state;
}

bool GameStateManager::quit()
{
	return (s32)m_gameStateCurr == GS_QUIT;
}

void GameStateManager::popState()
{
	m_gameStateNext = m_gameStatePrev;
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
	return m_gameStateCurr != m_gameStateNext;
}

void GameStateManager::nextState()
{
	// delete the old game states.
	if( m_gameStatePrev != m_gameStateCurr && m_gameStatePrev != m_gameStateNext )
		destroyState(m_gameStatePrev);

	m_gameStatePrev = m_gameStateCurr;
	m_gameStateCurr = m_gameStateNext;
}

void GameStateManager::nextState(IGameState * nextState)
{
	m_gameStateNext = nextState;
	nextState->setGameStateManager(this);
}

void GameStateManager::restartState()
{
	// make the next state the current state (instaed of GS_RESTART)
	m_gameStateNext = m_gameStateCurr;
}

void GameStateManager::loadState()
{
	m_gameStateCurr->load();
}

void GameStateManager::initState()
{
	m_gameStateCurr->init();
}

void GameStateManager::updateState()
{
	m_gameStateCurr->update();
}

void GameStateManager::drawState()
{
	m_gameStateCurr->draw();
}

void GameStateManager::freeState()
{
	m_gameStateCurr->free();
}

void GameStateManager::unloadState()
{
	m_gameStateCurr->unload();
}

