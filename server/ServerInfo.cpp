#include "ServerInfo.h"
#include <sstream>

ServerInfo::ServerInfo()
	: port(0), maxPlayers(0), currentPlayers(0)
{}

ServerInfo::~ServerInfo()
{}

std::string ServerInfo::info() const
{
	std::stringstream sinfo;

	sinfo << name << "     " << currentPlayers << " / " << maxPlayers;

	return sinfo.str();
}

ServerInfo & ServerInfo::operator=(const ServerInfo & rhs)
{
	memcpy(ip, rhs.ip, sizeof(ip));
	memcpy(name, rhs.name, sizeof(name));

	port = rhs.port;
	maxPlayers = rhs.maxPlayers;
	currentPlayers = rhs.currentPlayers;

	return *this;
}

std::istream & operator>>(std::istream & is, ServerInfo & info)
{
	bool success = true;

	is.getline( info.name, sizeof(info.name) );
	is.getline( info.ip, sizeof(info.ip) );
	is >> info.port;
	is >> info.maxPlayers;

	if( !success )
		is.setstate(std::ios::failbit);

	return is;
}
