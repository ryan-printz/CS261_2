#pragma once

#include "BaseNetMessage.h"
#include "PlayerReplicationInfo.h"

struct PlayerReplicationInfoNetMessage : public BaseNetMessage
{
public:
	PlayerReplicationInfoNetMessage(PlayerReplicationInfo & pri)
		: BaseNetMessage(PLAYER_REPLICATION_INFO), m_playerInfo(pri)
	{}

	PlayerReplicationInfo & playerInfo()
	{
		return m_playerInfo;
	}

protected:
	PlayerReplicationInfo m_playerInfo;
};
