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
	: m_isBlocking(false), 
	m_isInitialized(false), 
	m_isConnected(false), 
	m_socket(INVALID_SOCKET)
{}

//bool Socket::initializeTCP(const char * ipAddress, uint port, uint family)
//{
//
//
//	m_socket = socket(family, SOCK_STREAM, IPPROTO_TCP);
//
//	if( m_socket == INVALID_SOCKET )
//	{
//		m_error = WSAGetLastError();
//		return false;
//	}
//
//	return true;
//}

//bool Socket::initializeUDP(const char * ipAddress, uint port, uint family)
//{
//	m_address.sin_family = family;
//	m_address.sin_port = htons(port);
//	m_address.sin_addr.s_addr = inet_addr(ipAddress);
//
//	m_socket = socket(family, SOCK_DGRAM, IPPROTO_UDP);
//
//	if( m_socket == INVALID_SOCKET )
//	{
//		m_error = WSAGetLastError();
//		return false;
//	}
//
//	return true;
//}

//bool Socket::cleanUp()
//{
//	int error = 0;
//
//	if( m_isConnected )
//	{
//		error = shutdown(m_socket, SD_BOTH);
//		if( error )
//		{
//			m_error = WSAGetLastError();
//			return false;
//		}
//	}
//
//	error = closesocket(m_socket);
//	if( error )
//	{
//		m_error = WSAGetLastError();
//		return false;
//	}
//
//	return true;
//}

//bool Socket::bind()
//{
//	// bind the socket.
//	int error = ::bind(m_socket, (SOCKADDR*)&m_address, sizeof(m_address));
//
//	if( error )
//	{
//		m_error = WSAGetLastError();
//		return false;			
//	}
//
//	return true;
//}

//bool Socket::bind(const NetAddress * address)
//{
//	// bind the socket.
//	int error = ::bind(m_socket, (SOCKADDR*)address, sizeof(address));
//
//	if( error )
//	{
//		m_error = WSAGetLastError();
//		return false;			
//	}
//
//	return true;
//}

//bool Socket::connect()
//{
//	// connect the socket to an address;
//	int error = ::connect(m_socket, (SOCKADDR*)&m_address, sizeof(m_address));
//
//	if( error )
//	{
//		m_error = WSAGetLastError();
//		return false;
//	}
//
//	m_isConnected = true;
//	return true;
//}

//bool Socket::connect(const NetAddress * address)
//{
//	// connect the socket to an address;
//	int error = ::connect(m_socket, (SOCKADDR*)address, sizeof(address));
//
//	if( error )
//	{
//		m_error = WSAGetLastError();
//		return false;
//	}
//
//	m_isConnected = true;
//	return true;
//}

//bool Socket::listen(ubyte backlog)
//{
//	int error = ::listen(m_socket, backlog);
//
//	if( error )
//	{
//		m_error = WSAGetLastError();
//		return false;
//	}
//
//	return true;
//}

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

//int Socket::send(const ubyte * buffer, uint size)
//{
//	int sent = ::send(m_socket, (char*)buffer, size, 0);
//
//	if( sent == SOCKET_ERROR )
//		m_error = WSAGetLastError();
//	else
//		Sleep(1);
//
//	return sent;
//}

//int Socket::send(const ubyte * buffer, uint size, const NetAddress * address)
//{
//	int sent = ::sendto(m_socket, (char*)buffer, size, 0, (SOCKADDR*)address, sizeof(NetAddress));
//
//	if( sent == SOCKET_ERROR )
//		m_error = WSAGetLastError();
//
//	return sent;
//}

//int Socket::receive(ubyte * buffer, uint size)
//{
//	int received = ::recv(m_socket, (char*)buffer, size, 0);
//		
//	// connection closed
//	if( received == 0 )
//	{
//		m_isConnected = false;
//		return 0;
//	}
//	else if( received == SOCKET_ERROR )
//	{
//		m_error = WSAGetLastError();
//        if (m_error == WSAEWOULDBLOCK)
//            return 0; // for now I am using this for chat, after that fix this
//	}
//
//	return received;
//}

//int Socket::receive(ubyte * buffer, uint size, NetAddress * address)
//{
//	int addrSize = sizeof(NetAddress);
//	int received = ::recvfrom(m_socket, (char*)buffer, size, 0, (SOCKADDR*)address, &addrSize);
//
//	// connection closed
//	if( received == 0 )
//	{
//		m_isConnected = false;
//		return 0;
//	}
//	else if( received == SOCKET_ERROR )
//	{
//		m_error = WSAGetLastError();
//		return received;
//	}
//
//	return received;
//}

//Socket Socket::accept()
//{
//	Socket accepted;
//	int addrlen = sizeof(accepted.m_address);
//
//	// returns a new socket that the connection is actually made on.
//	accepted.m_socket = ::accept(m_socket, (SOCKADDR*)&accepted.m_address, &addrlen);
//
//	if( accepted.m_socket != INVALID_SOCKET ) {
//		accepted.m_isConnected = true;
//    }
//	else
//		m_error = WSAGetLastError();
//
//	return accepted;
//}
