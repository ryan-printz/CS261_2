#include "UDPSocket.h"

UDPSocket::UDPSocket()
{};

UDPSocket::~UDPSocket()
{}

bool UDPSocket::cleanup()
{
	return false;
}

bool UDPSocket::initialize(NetAddress address)
{
	return false;
}

bool connect(const NetAddress & to)
{
	return false;
}

bool listen(const NetAddress local, char backlog = 10)
{
	return false;
}

ISocket * accept(NetAddress & remote)
{
	return nullptr;
}

int send(const char * buffer, unsigned size)
{
	return 0;
}

int send(const char * buffer, unsigned size, const NetAddress & to)
{
	return 0;
}

int receive(char * buffer, unsigned size)
{
	return 0;
}

int receive(char * buffer, unsigned size, NetAddress & from)
{
	return 0;
}