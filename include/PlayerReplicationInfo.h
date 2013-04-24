#pragma once

// TODO: Implementation. Player specific information that needs to be replicated accross the clients.

class PlayerReplicationInfo
{
public:
	char m_name[12];
	short m_score;
	char m_lives;
	float m_x, m_y, m_rotation;
};
