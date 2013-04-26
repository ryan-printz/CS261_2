#pragma once

#include "GameState_BasePlay.h"
#include "GameReplicationInfo.h"
#include "PlayerReplicationInfo.h"
#include "ProtoConnection.h"

// ---------------------------------------------------------------------------

class GameState_NetworkPlay : public GameState_BasePlay
{
public:
	GameState_NetworkPlay(GameReplicationInfo &gri, std::vector<PlayerReplicationInfo> &pris, ProtoConnection * gameServer);
	virtual ~GameState_NetworkPlay();

	virtual void onEnd();
	virtual void init();
	virtual void draw();
	virtual void update();

private:
	GameReplicationInfo m_GRI;
	std::vector<PlayerReplicationInfo> m_PRIs;
	ProtoConnection * m_gameServer;
};