#include "TCPConnection.h"

#include <sstream>

TCPConnection::TCPConnection()
{}

TCPConnection::TCPConnection(ISocket * connection, NetAddress & remote)
{
	m_socket = connection;
	m_remote = remote;
}

TCPConnection::~TCPConnection()
{
	delete m_socket;
}

bool TCPConnection::accept(ISocket * listener)
{
	m_socket = listener->accept(m_remote);

	return m_socket != nullptr;
}

int TCPConnection::send(Packet & p)
{
	char buffer[p.MAX + 4];
	memcpy(buffer, &p.m_length, 4);
	memcpy(buffer + 4, p.m_buffer, p.m_length);
	return m_socket->send(buffer, p.m_length+4) - 4;
}

int TCPConnection::receive(Packet & p)
{
	int test = -1;
	m_socket->receive((char*)&test, 4, false);
	if(test < p.MAX)
		return p.m_length = m_socket->receive(p.m_buffer, test);
	else
		return test;
}

std::string TCPConnection::info() const
{
	std::stringstream info;

	info << "TCP connection to ";
	info << m_remote.ip();
	info << " on ";
	info << m_remote.port();

	return info.str();
}
