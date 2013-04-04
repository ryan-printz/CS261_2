#pragma once

class IServer 
{
	//TODO: Implement this Interface
	// needs common functionality between Server and MasterServer
public:
	virtual ~IServer() = 0;

	virtual void update() = 0;
};
