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
	StartMasterServer(MulticastSocket * mc, ServerInfo & info, IGameState * parent);

	virtual void update();
	virtual void draw();

private:
	MulticastSocket * m_multicast;
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
	StartGameServer(MulticastSocket * multicast, const ServerInfo & info, NetAddress & MasterServerAddress, IGameState * parent);

	virtual void update() {};
	virtual void draw() {};

private:
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

	config >> m_masterServerPort;
	config.ignore();
	config >> m_info;

	memcpy(m_info.ip, NetAddress::localIP(), 16);
	printf("Myip = %s \nGame - %s on port %d\nMaster server port - %d\n", m_info.ip, m_info.name, m_info.port, m_masterServerPort);
	
	NetAddress multicast("224.0.0.1", m_masterServerPort);
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
			m_parent->nextState(new StartGameServer(m_multicast, 
													m_info,
													master,
													m_parent));
		}
	}

	else if( --m_timeout <= 0 )
	{
		m_parent->nextState(new StartMasterServer(m_multicast, m_info, m_parent));
	}
}


void LookForMasterServerState::draw()
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "looking for master server...");
}

StartMasterServer::StartMasterServer(MulticastSocket * mc, ServerInfo & info, IGameState * parent)
	: IGameState::State(parent), m_info(info), m_multicast(mc)
{
	std::cout << "start master" << std::endl;
	m_info = info;

	NetAddress serverAddress(NetAddress::localIP(), info.port);

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

	Packet received;
	NetAddress from;

	received.m_length = m_multicast->receive(received.m_buffer, received.MAX, from);

	if( received.m_length > 0 )
	{
		BaseNetMessage * msg = reinterpret_cast<BaseNetMessage*>(received.m_buffer);

		if( msg->type() == SERVER_INFO )
		{
			Packet response;

			new (response.m_buffer) ServerInfoNetMessage(m_info);
			response.m_length = sizeof(ServerInfoNetMessage);

			// send the response message (a ServerInfo Message);
			m_multicast->send(response.m_buffer, response.m_length, from);

			// take note of the server.
			m_master->pushServer(msg->as<ServerInfoNetMessage>()->info(), from);
		}
	}
}

void StartMasterServer::draw()
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID SERVERS <>");

	int y = 50;

	const ServerVector & servers = m_master->servers();

	for(int i = 0; i < servers.size(); ++i)
		AEGfxPrint(40, y+=20, 0xFFFFFFFF, (s8*)servers[i].first.info().c_str());
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

StartGameServer::StartGameServer(MulticastSocket * multicast, const ServerInfo & info, NetAddress & masterServerAddress, IGameState * parent)
	: IGameState::State(parent), m_info(info)
{
	Packet serverInfo;

	new (serverInfo.m_buffer) ServerInfoNetMessage(m_info);
	serverInfo.m_length = sizeof(ServerInfoNetMessage);
	
	std::cout << "sending info to master server..." << std::endl;

	ISocket * listen = new ProtoSocket();

	listen->initialize(NetAddress(info.ip, info.port));
	listen->setBlocking(false);
	listen->listen(NetAddress(info.ip, info.port));
	

	auto manager = new ConnectionManager<ProtoConnection>();
	manager->setListener(listen);

	auto gsthread = new ProtoConnectionManagerProcessThread(manager);

	m_gameServer = new GameServer(gsthread);

	nextGameState(new GameState_Server(multicast, m_info, m_gameServer));
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