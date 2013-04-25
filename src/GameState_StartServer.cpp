#include "GameState_StartServer.h"
#include "GameState_Server.h"
#include "TCPConnectionManagerProcess.h"
#include "ISocket.h"
#include "TCPSocket.h"
#include "TCPConnection.h"
#include "ProtoConnectionManagerProcess.h"
#include "ProtoSocket.h"
#include "ProtoConnection.h"
#include "ConnectionManager.h"
#include "ServerInfoNetMessage.h"
#include "MasterServer.h"
#include "MulticastSocket.h"
#include <fstream>

struct LookForMasterServerState : public IGameState::State
{
public:
	LookForMasterServerState(const char * filename, IGameState * parent);

	virtual void update();
	virtual void draw();

private:
	ServerInfo m_info;
	char m_masterIp[16];
	unsigned m_masterServerPort;

	TCPSocket * m_socket;

	bool m_firstrun;
	MulticastSocket * m_multicast;
	int m_timeout;
};

struct StartMasterServer : public IGameState::State
{
public:
	StartMasterServer(NetAddress & serverAddress, ServerInfo & info, IGameState * parent);

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
	StartGameServer(const ServerInfo & info, NetAddress & MasterServerAddress, IGameState * parent);

	virtual void update() {};
	virtual void draw() {};

private:
	//IConnection * m_masterServer;
	GameServer * m_gameServer;
	ServerInfo m_info;
};

/////

LookForMasterServerState::LookForMasterServerState(const char * filename, IGameState * parent)
	: IGameState::State(parent), m_firstrun(true), m_timeout(90)
{
	std::ifstream config(filename);

	if( !config.is_open() )
		printf("Error opening config file. Config file not found.\n");

	config.getline( m_masterIp, sizeof(m_masterIp) );
	config >> m_masterServerPort;
	config.ignore();

	config >> m_info;

	printf("game - %s\n", m_info.name);
	
	NetAddress multicast((unsigned)INADDR_ANY, m_masterServerPort);
	m_multicast = new MulticastSocket();
	if( !m_multicast->initialize(multicast) )
		printf("Multicast failed to initialize: %i\n", WSAGetLastError() );

	m_multicast->setBlocking(false);

	// send out your info to announce.
	Packet p;

	new (p.m_buffer) ServerInfoNetMessage(m_info);
	p.m_length = sizeof(ServerInfoNetMessage);

	m_multicast->send(p.m_buffer, p.m_length);

	printf("looking for master server...\n");
};

void LookForMasterServerState::update()
{
	Packet received;
	NetAddress master;
	received.m_length = m_multicast->receive(received.m_buffer, received.MAX, master);

	if( received.m_length > 0 )
	{
		BaseNetMessage * msg = reinterpret_cast<BaseNetMessage*>(received.m_buffer);

		if( msg->type() == SERVER_INFO )
		{
			m_parent->nextState(new StartGameServer(msg->as<ServerInfoNetMessage>()->info(),
													master,
													m_parent));
			m_multicast->cleanup();
		}
	}

	else if( --m_timeout <= 0 )
	{
		m_parent->nextState(new StartMasterServer(NetAddress(m_masterIp, m_masterServerPort), 
							m_info, 
							m_parent));
		m_multicast->cleanup();
	}
}

void LookForMasterServerState::draw()
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "looking for master server...");
}

StartMasterServer::StartMasterServer(NetAddress & serverAddress, ServerInfo & info, IGameState * parent)
	: IGameState::State(parent), m_info(info)
{
	std::cout << "start master" << std::endl;

	ISocket * listen = new TCPSocket();

	listen->initialize(serverAddress);
	listen->listen(serverAddress);
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

StartGameServer::StartGameServer(const ServerInfo & info, NetAddress & masterServerAddress, IGameState * parent)
	: IGameState::State(parent), m_info(info)
{
	Packet serverInfo;

	//m_masterServer = new TCPConnection(socket, *masterServerAddress);

	new (serverInfo.m_buffer) ServerInfoNetMessage(m_info);
	serverInfo.m_length = sizeof(ServerInfoNetMessage);

	//m_masterServer->send(serverInfo);
	
	std::cout << "sending info to master server..." << std::endl;

	ISocket * listen = new ProtoSocket();

	listen->initialize(NetAddress(info.ip, info.port));
	listen->setBlocking(false);
	listen->listen(NetAddress(info.ip, info.port));
	

	auto manager = new ConnectionManager<ProtoConnection>();
	manager->setListener(listen);

	auto gsthread = new ProtoConnectionManagerProcessThread(manager);

	m_gameServer = new GameServer(gsthread);

	//this->nextGameState(new GameState_Server((TCPConnection*)m_masterServer, m_info, m_gameServer));
	//parent->nextState(new GameState_Server((TCPConnection*)m_masterServer, m_info));
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