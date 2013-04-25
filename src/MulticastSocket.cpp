#include "MulticastSocket.h"
#include <iostream>
#include <fstream>

MulticastSocket::MulticastSocket()
{}

MulticastSocket::~MulticastSocket()
{}

bool MulticastSocket::cleanup()
{
	::shutdown(m_socket, SD_BOTH);
	return ::closesocket(m_socket) != SOCKET_ERROR;
}

bool MulticastSocket::addToMulticastGroup(NetAddress member)
{
	if( ::setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)member.ip(), sizeof(NetAddress)) )
		return false;
}

bool MulticastSocket::initialize(NetAddress address)
{
	m_socket = socket(address.sin_family, SOCK_DGRAM, IPPROTO_UDP);

	if( !(m_isInitialized = (m_socket != INVALID_SOCKET)) )
		return false;

	if( ::bind(m_socket, (SOCKADDR*)&address, sizeof(address)) )
		return false;

	m_multicast.imr_multiaddr.s_addr = inet_addr(address.ip());
	
	m_end.sin_addr = m_multicast.imr_multiaddr;
	m_end.sin_family = AF_INET;
	m_end.sin_port = address.sin_port;

	m_multicast.imr_multiaddr.s_addr = inet_addr(NetAddress::localIP());
	::setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&m_multicast, sizeof(m_multicast)); 

	int ttl = 1;
	::setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&ttl, sizeof(ttl));

	int loopback = 0;
	::setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_LOOP, (const char*)&loopback, sizeof(loopback));

	return m_isInitialized = (m_socket != INVALID_SOCKET);
}

int MulticastSocket::send(const char * buffer, unsigned size, bool write)
{
	int sent = ::sendto(m_socket, buffer, size, 0, (SOCKADDR*)&m_end, sizeof(m_end));

	if(write)
	{
		FILE* myFile;
		myFile = fopen ("log.txt", "a");
		fwrite("Multicast Send: ", 1, 16, myFile);
		fwrite(buffer, 1, size, myFile);
		fwrite("\n", 1, 1, myFile);
		fclose(myFile);
	}

	return sent;
}

int MulticastSocket::receive(char * buffer, unsigned size, NetAddress & from, bool write)
{
	int fromSize = sizeof(from);
	int recvd = ::recvfrom(m_socket, buffer, size, 0, (SOCKADDR*)&from, &fromSize);

	if(write)
	{
		FILE* myFile;
		myFile = fopen ("log.txt", "a");
		fwrite("Multicast Receive: ", 1, 19, myFile);
		fwrite(buffer, 1, size, myFile);
		fwrite("\n", 1, 1, myFile);
		fclose(myFile);
	}	

	return recvd;	
}

int MulticastSocket::receive(char * buffer, unsigned size, bool write)
{
	return receive(buffer, size, m_recvd, write);
}