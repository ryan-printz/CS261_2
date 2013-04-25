#include "TCPSocket.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

TCPSocket::TCPSocket()
{}

TCPSocket::TCPSocket(SOCKET accepted)
{
	m_isConnected = m_isInitialized = true;
	m_socket = accepted;
}

TCPSocket::~TCPSocket()
{}

bool TCPSocket::cleanup()
{
	if( m_isInitialized && shutdown(m_socket, SD_BOTH) )
	{
		m_isConnected =	m_isInitialized = m_isBlocking = false;
		return false;
	}

	if( closesocket(m_socket) )
		return false;

	return true;
}

bool TCPSocket::initialize(NetAddress address)
{
	m_socket = socket(address.sin_family, SOCK_STREAM, IPPROTO_TCP);

	return m_isInitialized = (m_socket != INVALID_SOCKET);
}

bool TCPSocket::connect(const NetAddress & to)
{
	if( !m_isInitialized || ::connect(m_socket, (SOCKADDR*)&to, sizeof(to)) )
		return false;

	return m_isConnected = true;
}

bool TCPSocket::listen(const NetAddress local, char backlog) 
{
	if ( ::bind(m_socket, (SOCKADDR*)&local, sizeof(local)) )
		return false;

	if ( ::listen(m_socket, backlog) )
		return false;

	return true;
}

ISocket * TCPSocket::accept(NetAddress & remote) 
{
	SOCKET accepted = INVALID_SOCKET;
	ISocket * newSocket = nullptr;
	int addrlen = sizeof(remote);

	accepted = ::accept(m_socket, (SOCKADDR*)&remote, &addrlen);
	if(accepted != INVALID_SOCKET)
		newSocket = new TCPSocket(accepted);

	return newSocket;
}

int TCPSocket::send(const char * buffer, unsigned size, bool write)
{
	int sent = ::send(m_socket, buffer, size, 0);
	if(write)
	{
		FILE* myFile;
		myFile = fopen ("log.txt", "a");
		fwrite("Sending TCP: ", 1, 13, myFile);
		fwrite(buffer, 1, size, myFile);
		fwrite("\n", 1, 1, myFile);
		fclose(myFile);
	}	
	return sent;//::send(m_socket, buffer, size, 0);
}

int TCPSocket::send(const char * buffer, unsigned size, const NetAddress &, bool write)
{
	return send(buffer, size, write);
}

int TCPSocket::receive(char * buffer, unsigned size, bool write)
{
	int rcvd = ::recv(m_socket, buffer, size, 0);
	if(rcvd > 0 && write)
	{
		int j = 5;
		FILE* myFile;
		myFile = fopen ("log.txt", "a");
		fwrite("Received TCP: ", 1, 14, myFile);
		fwrite(buffer, 1, size, myFile);
		fwrite("\n", 1, 1, myFile);
		fclose(myFile);
	}
	return rcvd;
}

int TCPSocket::receive(char * buffer, unsigned size, NetAddress &, bool write)
{
	return receive(buffer, size, write);
}
