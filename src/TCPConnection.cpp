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
	return m_socket->send(p.m_buffer, p.m_length);
}

int TCPConnection::receive(Packet & p)
{
	return p.m_length = m_socket->receive(p.m_buffer, Packet::MAX);
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
