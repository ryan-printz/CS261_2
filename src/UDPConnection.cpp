#include "UDPConnection.h"
#include "UDPSocket.h"

#include <sstream>

UDPConnection::UDPConnection()
{}

UDPConnection::UDPConnection(ISocket * connection, NetAddress & remote)
{
	m_socket = connection;
	m_remote = remote;
}

UDPConnection::~UDPConnection()
{
	delete m_socket;
}

bool UDPConnection::accept(ISocket * listener)
{
	UDPSocket* uSocket = dynamic_cast<UDPSocket*>(listener);
	
	if(!uSocket)
		return false;

	UDPSocket accepted = uSocket->acceptUDP();

	m_socket = listener->accept(m_remote);

	return m_socket != nullptr;
}

int UDPConnection::send(Packet & p)
{
	return m_socket->send(p.m_buffer, p.m_length);
}

int UDPConnection::receive(Packet & p)
{
	return p.m_length = m_socket->receive(p.m_buffer, Packet::MAX);
}

std::string UDPConnection::info() const
{
	std::stringstream info;

	info << "UDP connection to ";
	info << m_remote.ip();
	info << " on ";
	info << m_remote.port();

	return info.str();
}
