#include "main.h"
#include "GameState_Server.h"

#include "BaseNetMessage.h"

// TODO: refactor this, GameState_NetPlay, and GameState_Play to use a common base class.
// TODO: refactor game objects
// TODO: Server-side debugging/controls. It would be nice to add gui to this, but we may not have access to the window.
// TODO: Network engine
// TODO: Relevance/zoning.
// TODO: Server State.
// TODO: Game Replication Info

GameState_Server::GameState_Server(MulticastSocket * master, ServerInfo & info, GameServer* gameServer)
	: m_master(master), m_gameServer(gameServer)
{
	gameServer->getInfo() = info;
}

GameState_Server::~GameState_Server()
{}

void GameState_Server::init()
{
	// reset the number of current asteroid and the total allowed
	m_game.m_asteroids = 0;
	m_game.m_maxAsteroids = AST_NUM_MIN;

	// get the time the asteroid is created
	m_game.m_asteroidTimer = AEGetTime();
	
	// generate the initial asteroid
	for (u32 i = 0; i < m_game.m_maxAsteroids; i++)
		m_game.astCreate(0);

	// reset
	m_gameServer->getGRI().m_inProgress = true;
	m_gameServer->getGRI().m_wave = 0;

	for( auto pri = m_gameServer->getPRIs().begin(); pri != m_gameServer->getPRIs().end(); ++pri )
	{
		// reset the score and the number of ship
		pri->m_score = 0;
		pri->m_lives = SHIP_INITIAL_NUM;
	}

	// reset the delay to restart after game over.
	m_game.m_gameTimer = 2.0f;
}

void GameState_Server::update()
{
	m_gameServer->update();
	// update the server using only the info from the clients.
	// TODO: update with network info
}

void GameState_Server::unload()
{
	Packet disconnect;

	new (disconnect.m_buffer) BaseNetMessage(DISCONNECT);
	disconnect.m_length = sizeof(BaseNetMessage);

	m_master->send(disconnect.m_buffer, disconnect.m_length);
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

	// draw the hud
	sprintf(strBuffer, "Level: %d", m_gameServer->getGRI().m_wave);
	AEGfxPrint(10, 30, 0xFFFFFFFF, strBuffer);

	ServerInfo & info = m_gameServer->getInfo();
	sprintf(strBuffer, "%s (%s:%i) %i/%i", info.name, info.ip, info.port, info.currentPlayers, info.maxPlayers);
	AEGfxPrint(150, 10, 0xFFFFFFFF, strBuffer);

	int y = 10;
	auto pris = m_gameServer->getPRIs();
	for(auto pri = pris.begin(); pri != pris.end(); ++pri)
	{
		sprintf(strBuffer, "%i %i %s %i", pri->m_lives, pri->m_netid, pri->m_name, pri->m_score);
		AEGfxPrint(600, y += 20, 0xFFCCCCCC, strBuffer);

		// visually represent update area
		AEGfxSphere(pri->m_x, pri->m_y, 99, 30);
	}
}