#pragma once

#include "GameState_BasePlay.h"
#include "GameReplicationInfo.h"
#include "PlayerReplicationInfo.h"
#include "UDPConnection.h"

// ---------------------------------------------------------------------------

class GameState_NetworkPlay : public GameState_BasePlay
{
public:
	GameState_NetworkPlay(GameReplicationInfo &gri, std::vector<PlayerReplicationInfo> &pris, UDPConnection * gameServer);
	virtual ~GameState_NetworkPlay();

	virtual void onEnd();
	virtual void init();
	virtual void draw();

private:
	GameReplicationInfo m_GRI;
	std::vector<PlayerReplicationInfo> m_PRIs;
	UDPConnection * m_gameServer;
};