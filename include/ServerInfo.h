#pragma once

#include <string>

struct ServerInfo
{
	ServerInfo();
	~ServerInfo();

	char ip[16];
	short port;

	char name[32];
	short currentPlayers;
	short maxPlayers;

	std::string info() const;
	ServerInfo & operator=(const ServerInfo & rhs);
};

std::istream & operator>>(std::istream & is, ServerInfo & info);