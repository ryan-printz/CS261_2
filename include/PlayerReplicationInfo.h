#pragma once

// TODO: Implementation. Player specific information that needs to be replicated accross the clients.

#define INVALID_NET_ID -1

class PlayerReplicationInfo
{
public:
	PlayerReplicationInfo()
		: m_netid(INVALID_NET_ID)
	{}

	// net id is assigned by the server to each client.
	// the client keeps track of their own id, and it is
	// used on the server to associate a player to their PRI.
	int m_netid;
	char m_name[12];
	short m_score;
	char m_lives;
	float m_x, m_y, m_rotation;
};
