#pragma once

#include "IServer.h"
#include "ServerInfo.h"
#include "GameReplicationInfo.h"
#include "PlayerReplicationInfo.h"
#include "ObjectNetMessage.h"

#include <list>
#include <vector>

class ProtoConnection;
class ProtoConnectionManagerProcessThread;
class PlayerReplicationInfo;

// TODO: this needs to implement and inherit from IServer
// possibly needs common functionality implemented in a BaseServer class.
// needs to use UDP sockets/connections to connect to players.
typedef std::vector<PlayerReplicationInfo> PRIVector;
typedef std::list<ObjectNetMessage> ObjectMsgList;

class GameServer : public IServer
{
public:
	GameServer() {}
	GameServer(ProtoConnectionManagerProcessThread * gsThread);
	virtual ~GameServer();
	virtual void update();

	PRIVector & getPRIs();
	GameReplicationInfo & getGRI();
	ServerInfo & getInfo();
	int getNextNetID();

	std::list<ProtoConnection*> & getNewConnections();
	ObjectMsgList & getObjectMsgs();
	
protected:
	void addNewPlayer(ProtoConnection * connected, PlayerReplicationInfo & pri);
	bool updatePlayerReplicationInfo(PlayerReplicationInfo & pri);

	int m_nextNetID;
	ServerInfo m_info;

	std::list<ProtoConnection*> m_newConnections;
	ProtoConnectionManagerProcessThread * m_gsThread;

	GameReplicationInfo m_GRI;
	PRIVector m_PRIs;
	ObjectMsgList m_objectMsgs;
};
