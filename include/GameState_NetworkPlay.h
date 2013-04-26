#pragma once

#include "GameState_BasePlay.h"
#include "GameReplicationInfo.h"
#include "PlayerReplicationInfo.h"
#include "ProtoConnection.h"
#include "NetObjectManager.h"
#include "ObjectNetMessage.h"

// ---------------------------------------------------------------------------

class GameState_NetworkPlay : public GameState_BasePlay
{
public:
	GameState_NetworkPlay(GameReplicationInfo &gri, std::vector<PlayerReplicationInfo> &pris, ProtoConnection * gameServer, int netID);
	virtual ~GameState_NetworkPlay();

	virtual void onEnd();
	virtual void init();
	virtual void draw();
	virtual void unload();
	virtual void update();
	void updatePRI(PlayerReplicationInfo& pri);
	void push(ObjectNetMessage& obj);

private:
	GameReplicationInfo m_GRI;
	NetObjectManager m_netObjects;
	std::vector<PlayerReplicationInfo> m_PRIs;
	ProtoConnection * m_gameServer;
	int m_netID;
	int m_send;
	float m_lastRecv;
};