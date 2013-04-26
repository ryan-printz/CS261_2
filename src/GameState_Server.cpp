#include "main.h"
#include "GameState_Server.h"

#include "BaseNetMessage.h"
#include "ServerInfoNetMessage.h"

// TODO: refactor this, GameState_NetPlay, and GameState_Play to use a common base class.
// TODO: refactor game objects
// TODO: Server-side debugging/controls. It would be nice to add gui to this, but we may not have access to the window.
// TODO: Network engine
// TODO: Relevance/zoning.
// TODO: Server State.
// TODO: Game Replication Info

GameState_Server::GameState_Server(MulticastSocket * master, ServerInfo & info, GameServer* gameServer)
	: m_master(master), m_gameServer(gameServer), m_netObjects(&m_game)
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
	//for (u32 i = 0; i < m_game.m_maxAsteroids; i++)
	//	m_game.astCreate(0);

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
	static int timer = 0;

	m_gameServer->update();

	if ((m_game.m_asteroids < m_game.m_maxAsteroids) && ((AEGetTime() - m_game.m_asteroidTimer) > AST_CREATE_DELAY))
	{
		// keep track the last time an asteroid is created
		m_game.m_asteroidTimer = AEGetTime();

		// create an asteroid
		auto gObj = m_game.astCreate(0);

		gObj->velCurr.x = float(rand() % 100) / 100.f * 20.f - 10.f;
		gObj->velCurr.y = float(rand() % 100) / 100.f * 20.f - 10.f;
		gObj->scale = 15.f;
		m_Asteroids.emplace(std::make_pair<short, GameObjInst*>(m_gameServer->getNextNetID(), gObj ));
	}

	if( !(++timer % 10) )
	{
		Packet serverInfo;
	
		new (serverInfo.m_buffer) ServerInfoNetMessage(m_gameServer->getInfo());
		serverInfo.m_length = sizeof(ServerInfoNetMessage);

		//m_master->send(serverInfo.m_buffer, serverInfo.m_length);
	}
	
	// update the server using only the info from the clients.
	auto objects = m_gameServer->getObjectMsgs();
	for(auto object = objects.begin(); object != objects.end(); ++object)
	{
		m_netObjects.update(object->netId, object->type, object->flags, object->x, object->y, object->z, object->velx, object->vely);
	}
	objects.clear();

	auto test = m_Asteroids.begin();
	for(; test != m_Asteroids.end(); ++test)
	{
		AEVec2 u;
		f32    uLen;

		// warp the asteroid from one end of the screen to the other
		test->second->posCurr.x = AEWrap(test->second->posCurr.x, gAEWinMinX - AST_SIZE_MAX, gAEWinMaxX + AST_SIZE_MAX);
		test->second->posCurr.y = AEWrap(test->second->posCurr.y, gAEWinMinY - AST_SIZE_MAX, gAEWinMaxY + AST_SIZE_MAX);
		test->second->posCurr.x += test->second->velCurr.x * gAEFrameTime;
		test->second->posCurr.y += test->second->velCurr.y * gAEFrameTime;
		auto connect = m_gameServer->getNewConnections().begin();
		for(; connect != m_gameServer->getNewConnections().end(); ++connect)
		{
			Packet playerInfo;
			new (playerInfo.m_buffer) ObjectNetMessage(test->first, TYPE_ASTEROID, FLAG_ACTIVE, test->second->posCurr.x, test->second->posCurr.y, test->second->dirCurr, test->second->velCurr.x, test->second->velCurr.y);
			playerInfo.m_length = sizeof(ObjectNetMessage);


			(*connect)->send(playerInfo);

		}
	}
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
		AEGfxPrint(450, y += 20, 0xFFCCCCCC, strBuffer);

		// visually represent update area
		AEGfxSphere(pri->m_x, pri->m_y, 99, 30);
	}
}