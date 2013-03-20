#include "GameState_StartServer.h"
#include "TCPConnectionManagerProcess.h"
#include "ISocket.h"
#include "TCPSocket.h"
#include "TCPConnection.h"
#include "ConnectionManager.h"
#include <fstream>

struct LookForMasterServerState : public IGameState::State
{
public:
	LookForMasterServerState(const char * filename, IGameState * parent);

	virtual void update();
	virtual void draw();

private:
	NetAddress * m_serverAddress;
	int m_gameServerPort;

	TCPSocket * m_socket;

	bool m_firstrun;
};

struct StartMasterServer : public IGameState::State
{
public:
	StartMasterServer(NetAddress * serverAddress, int port, IGameState * parent);

	virtual void update();
	virtual void draw();

private:
	TCPConnectionManagerProcessThread * m_cmthread;
};

struct TimeoutState : public IGameState::State
{
public:
	TimeoutState(IGameState * parent);

	virtual void update();
	virtual void draw();
};

struct StartGameServer : public IGameState::State
{
public:
	StartGameServer(TCPSocket * socket, int port, IGameState * parent);

	virtual void update() {};
	virtual void draw() {};
};

/////

LookForMasterServerState::LookForMasterServerState(const char * filename, IGameState * parent)
	: IGameState::State(parent), m_firstrun(true)
{
	std::ifstream config(filename);

	if( !config.is_open() )
		printf("Error opening config file. Config file not found.\n");

	int port;
	char buffer[256];

	config.getline(buffer, 256);
	config >> port;
	config >> m_gameServerPort;
	config.close();

	printf("%s:%i:%i\n", buffer, port, m_gameServerPort);

	m_serverAddress = new NetAddress(buffer, port);
};

void LookForMasterServerState::update()
{
	if( m_firstrun )
	{
		m_firstrun = false;
		return;
	}

	printf("looking for master server...\n");
	m_socket = new TCPSocket;
	m_socket->initialize(*m_serverAddress);
	
	IGameState::State * nextState = nullptr;

	// attempting to connect to server
	if( !m_socket->connect(*m_serverAddress) )
	{
		printf("failed to connect to master server.\n");

		m_socket->cleanup();
		delete m_socket;

		if(0 == std::string("127.0.0.1").compare(m_serverAddress->ip()))	
			nextState = new StartMasterServer(m_serverAddress, m_gameServerPort, m_parent);
		else
			nextState = new TimeoutState(m_parent);
	}
	else
	{
		nextState = new StartGameServer(m_socket, m_gameServerPort, m_parent);
	}

	m_parent->nextState( nextState );
}

void LookForMasterServerState::draw()
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "looking for master server...");
}

StartMasterServer::StartMasterServer(NetAddress * serverAddress, int port, IGameState * parent)
	: IGameState::State(parent)
{
	printf("start master");

	ISocket * listen = new TCPSocket();

	listen->initialize(*serverAddress);
	listen->listen(*serverAddress);
	listen->setBlocking(false);

	auto manager = new ConnectionManager<TCPConnection>();
	manager->setListener(listen);

	m_cmthread = new TCPConnectionManagerProcessThread(manager);
}

void StartMasterServer::update()
{
}

void StartMasterServer::draw()
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID SERVER <>");
	
	m_cmthread->update();
}

TimeoutState::TimeoutState(IGameState * parent)
	: IGameState::State(parent)
{
	printf("timeout");
}

void TimeoutState::update()
{
	if(AEInputCheckTriggered(DIK_SPACE))
		m_parent->nextState(new LookForMasterServerState("config.txt", m_parent));
}

void TimeoutState::draw()
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "Connection timed out!");
	AEGfxPrint(40, 50, 0xFFFFFFFF, "Reattempt");

	if (gAEFrameCounter & 0x0008)
		AEGfxPrint(10, 50, 0xFFFFFFFF, ">>");
}

StartGameServer::StartGameServer(TCPSocket * socket, int port, IGameState * parent)
	: IGameState::State(parent)
{
	printf("start game server");
}

////////////////////////////

GameState_StartServer::GameState_StartServer()
{}

void GameState_StartServer::load()
{}

void GameState_StartServer::init()
{
	m_state = new LookForMasterServerState("config.txt", this);
}

void GameState_StartServer::update()
{
	m_state->update();
}

void GameState_StartServer::draw()
{
	m_state->draw();
}

void GameState_StartServer::free()
{}

void GameState_StartServer::unload()
{}