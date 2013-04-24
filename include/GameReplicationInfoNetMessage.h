#pragma once

#include "BaseNetMessage.h"
#include "GameReplicationInfo.h"

struct GameReplicationInfoNetMessage : public BaseNetMessage
{
	GameReplicationInfoNetMessage(GameReplicationInfo & gri)
		: BaseNetMessage(GAME_REPLICATION_INFO), m_gameInfo(gri)
	{}

	GameReplicationInfo & gameInfo()
	{
		return m_gameInfo;
	}

protected:
	GameReplicationInfo m_gameInfo;
};
