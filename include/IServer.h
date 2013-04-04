#pragma once

class IServer 
{
	//TODO: Implement this Interface
	// needs common functionality between Server and MasterServer
public:
	virtual ~IServer() {};

	virtual void update() = 0;
};
