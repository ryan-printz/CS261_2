#include "BaseConnection.h"

BaseConnection::BaseConnection()
	: m_socket(nullptr), m_remote()
{}

bool BaseConnection::connected() const
{
	return m_socket && m_socket->connected();
}

const NetAddress & BaseConnection::remote() const
{
	return m_remote;
}

void BaseConnection::push_sendPacket(Packet & p)
{
	m_send.push_back(p);
}

void BaseConnection::push_receivePacket(Packet & p)
{
	m_receive.push_back(p);
}

bool BaseConnection::pop_sendPacket(Packet & out)
{
	if( m_send.empty() )
		return false;

	out = m_send.front();
	m_send.pop_front();

	return true;
}

bool BaseConnection::pop_receivePacket(Packet & out)
{
	if( m_receive.empty() )
		return false;

	out = m_receive.front();
	m_receive.pop_front();

	return true;
}