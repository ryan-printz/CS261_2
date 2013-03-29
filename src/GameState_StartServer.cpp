#include "GameState_StartServer.h"
#include "TCPConnectionManagerProcess.h"
#include "ISocket.h"
#include "TCPSocket.h"
#include "TCPConnection.h"
#include "ConnectionManager.h"
#include "ServerInfoNetMessage.h"
#include "MasterServer.h"
#include <fstream>

struct LookForMasterServerState : public IGameState::State
{
public:
	LookForMasterServerState(const char * filename, IGameState * parent);

	virtual void update();
	virtual void draw();

private:
	NetAddress * m_serverAddress;
	ServerInfo m_info;
	char m_masterIp[16];
	unsigned m_masterServerPort;

	TCPSocket * m_socket;

	bool m_firstrun;
};

struct StartMasterServer : public IGameState::State
{
public:
	StartMasterServer(NetAddress * serverAddress, ServerInfo & info, IGameState * parent);

	virtual void update();
	virtual void draw();

private:
	MasterServer * m_master;
	ServerInfo m_info;
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
	StartGameServer(TCPSocket * socket, ServerInfo & info, NetAddress * MasterServerAddress, IGameState * parent);

	virtual void update() {};
	virtual void draw() {};

private:
	IConnection * m_masterServer;
	ServerInfo m_info;
};

/////

LookForMasterServerState::LookForMasterServerState(const char * filename, IGameState * parent)
	: IGameState::State(parent), m_firstrun(true)
{
	std::ifstream config(filename);

	if( !config.is_open() )
		printf("Error opening config file. Config file not found.\n");

	config.getline( m_masterIp, sizeof(m_masterIp) );
	config >> m_masterServerPort;
	config.ignore();

	config >> m_info;

	printf("master - %s:%i\ngame - %s:%i\n", m_masterIp, m_masterServerPort, m_info.ip, m_info.port);
	
	m_serverAddress = new NetAddress(m_masterIp, m_masterServerPort);
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

		if(0 == std::string(m_info.ip).compare(m_masterIp))	
			nextState = new StartMasterServer(m_serverAddress, m_info, m_parent);
		else
			nextState = new TimeoutState(m_parent);
	}
	else
	{
		nextState = new StartGameServer(m_socket, m_info, m_serverAddress, m_parent);
	}

	m_parent->nextState( nextState );
}

void LookForMasterServerState::draw()
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "looking for master server...");
}

StartMasterServer::StartMasterServer(NetAddress * serverAddress, ServerInfo & info, IGameState * parent)
	: IGameState::State(parent), m_info(info)
{
	std::cout << "start master" << std::endl;

	ISocket * listen = new TCPSocket();

	listen->initialize(*serverAddress);
	listen->listen(*serverAddress);
	listen->setBlocking(false);

	auto manager = new ConnectionManager<TCPConnection>();
	manager->setListener(listen);

	auto cmthread = new TCPConnectionManagerProcessThread(manager);

	m_master = new MasterServer(cmthread);
}

void StartMasterServer::update()
{
	m_master->update();
}

void StartMasterServer::draw()
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID SERVERS <>");
	int y = 50;
	for(int i = 0; i < m_master->serverCount(); ++i)
		AEGfxPrint(40, y+=20, 0xFFFFFFFF, (s8*)m_master->server(i).info().c_str());
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

StartGameServer::StartGameServer(TCPSocket * socket, ServerInfo & info, NetAddress * masterServerAddress, IGameState * parent)
	: IGameState::State(parent), m_info(info)
{
	Packet serverInfo;

	m_masterServer = new TCPConnection(socket, *masterServerAddress);

	new (serverInfo.m_buffer) ServerInfoNetMessage(m_info);
	serverInfo.m_length = sizeof(ServerInfoNetMessage);

	m_masterServer->send(serverInfo);
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