#include "UDPSocket.h"
#include <iostream>
#include <fstream>

UDPSocket::UDPSocket()
{}

UDPSocket::~UDPSocket()
{}

bool UDPSocket::cleanup()
{
	::shutdown(m_socket, SD_BOTH);
	return ::closesocket(m_socket) != SOCKET_ERROR;
}

bool UDPSocket::initialize(NetAddress address)
{
	m_socket = socket(address.sin_family, SOCK_DGRAM, IPPROTO_UDP);

	return m_isInitialized = (m_socket != INVALID_SOCKET);
}

int UDPSocket::send(const char * buffer, unsigned size, const NetAddress & to, bool write)
{
	int err = WSAGetLastError();
	int sent = ::sendto(m_socket, buffer, size, 0, (SOCKADDR*)&to, sizeof(to));
	err = WSAGetLastError();
	if(write) log("log.txt", "UDP Send: ", 10, buffer, size);

	return sent;
}

int UDPSocket::receive(char * buffer, unsigned size, NetAddress & from, bool write)
{
	int fromSize = sizeof(from);
	int recvd = ::recvfrom(m_socket, buffer, size, 0, (SOCKADDR*)&from, &fromSize);
	if(write && recvd > 0) 
		log("log.txt", "UDP Receive: ", 13, buffer, size);

	return recvd;	
}

int UDPSocket::receive(char * buffer, unsigned size, bool write)
{
	return receive(buffer, size, m_from, write);
}

int UDPSocket::send(const char * buffer, unsigned size, bool write)
{
	return send(buffer, size, m_to, write);
}