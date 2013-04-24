#include "main.h"
#include "GameState_Server.h"

// TODO: refactor this, GameState_NetPlay, and GameState_Play to use a common base class.
// TODO: refactor game objects
// TODO: Server-side debugging/controls. It would be nice to add gui to this, but we may not have access to the window.
// TODO: Network engine
// TODO: Relevance/zoning.
// TODO: Server State.
// TODO: Game Replication Info

GameState_Server::GameState_Server(TCPConnection * master, ServerInfo & info, GameServer* gameServer)
	: m_info(info), m_master(master), m_gameServer(gameServer)
{}

GameState_Server::~GameState_Server()
{}

void GameState_Server::update()
{
	m_gameServer->update();
	// update the server using only the info from the clients.

	// TODO: update with network info

}

void GameState_Server::draw()
{	
	// draw the game like normal.
	s8 strBuffer[1024];

	// draw all object in the list
	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = &m_game.m_gameObjInsts[i];

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		
		AEGfxSetTransform	(&pInst->transform);
		AEGfxTriDraw		(pInst->pObject->pMesh);
	}

	// draw debug info

	// TODO: debug info
}