#pragma once

// TODO: Implement Game Replication Info
// the catchall class for information that needs to be synced between clients, plus client info.

// TODO: needs a method for serializing itself/deserializing itself.

// TODO: possibly needs to be extracted into a common interface/base class implementation "IReplicationInfo"

class GameReplicationInfo
{
public:
	bool m_inProgress;
	char m_PRICount;
};
