#include "BaseSocket.h"
#include <iostream>

#pragma comment(lib, "WS2_32.lib")

bool initSockets(bool printInfo)
{
	static WSAData s_wsData;
	int result;

	result = WSAStartup(MAKEWORD(2,2), &s_wsData);

	if( result )
	{
		if( printInfo ) std::cout << "WinSock Startup Failed. #" << WSAGetLastError() << "!" << std::endl;
		return false;
	}

	if( printInfo ) std::cout << "WinSock 2.2 initialized!" << std::endl;

	return true;
}

bool cleanSockets()
{
	return WSACleanup() != SOCKET_ERROR;
}

/////////////////////

NetAddress::NetAddress()
{
	sin_family = AF_INET;
}

NetAddress::NetAddress(unsigned ip, unsigned port)
{
	sin_family = AF_INET;
	sin_port = htons(port);
	sin_addr.s_addr = ip;
}

NetAddress::NetAddress(unsigned port)
{
	sin_family = AF_INET;
	sin_port = htons(port);
	sin_addr.s_addr = inet_addr(localIP());
}

NetAddress::NetAddress(const char * ip, unsigned port)
{
	sin_family = AF_INET;
	sin_port = htons(port);
	sin_addr.s_addr = inet_addr(ip);
}

const char * NetAddress::ip() const
{
	return inet_ntoa(sin_addr); 
}

unsigned NetAddress::port() const
{
	return ntohs(sin_port);
}

char * NetAddress::localIP()
{
	// get the ip for localhost.
	hostent * entity = gethostbyname("");

	return inet_ntoa( *(in_addr*)*entity->h_addr_list );
}

/////////////////////

BaseSocket::BaseSocket() 
	: m_isBlocking(true), 
	m_isInitialized(false), 
	m_isConnected(false), 
	m_socket(INVALID_SOCKET)
{}

void BaseSocket::setBlocking(bool blocking)
{
	// socket is already setup correctly.
	if( m_isBlocking == blocking )
		return;

	// enable/disable non-blocking mode.
	unsigned long val = (blocking)?0:1;
	if( ioctlsocket(m_socket, FIONBIO, &val) == SOCKET_ERROR )
		return;

	m_isBlocking = blocking;
	return;
}

bool BaseSocket::invalid() const
{
	return m_socket == INVALID_SOCKET;
}

bool BaseSocket::blocking() const
{
	return m_isBlocking;
}

bool BaseSocket::initialized() const
{
	return m_isInitialized;
}

bool BaseSocket::connected() const
{
	return m_isConnected;
}

unsigned BaseSocket::lastError() const
{
	return WSAGetLastError();
}
