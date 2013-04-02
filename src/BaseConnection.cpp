#include "BaseConnection.h"
#include <iostream>
#include "BaseNetMessage.h"

BaseConnection::BaseConnection()
	: m_socket(nullptr), m_remote(), m_sendRate(0.0f), m_receiveRate(0.0f), m_sendTimer(0.0f), m_receiveTimer(0.0f)
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

void BaseConnection::update(float dt)
{
	if( m_receiveTimer += dt > m_receiveRate )
	{
		get();
		m_receiveTimer -= m_receiveRate;
	}

	if( m_sendTimer += dt > m_sendRate )
	{
		set();
		m_sendTimer -= m_sendRate;
	}
}

void BaseConnection::get()
{
	Packet in;

	if( 0 > receive(in) )
		 return;

	if( in.m_length == 0 )
		new (in.m_buffer) BaseNetMessage(DISCONNECT);

	push_receivePacket(in);
}

void BaseConnection::set()
{
	Packet out;

	if( pop_sendPacket(out) )
		if( out.m_length != send(out) )
			std::cout << "failed to send packet correctly. Packet Length" << out.m_length
			<< " Packet: " << out.m_buffer << std::endl;
}

void BaseConnection::setReceiveRate(float rate)
{
	m_receiveRate = rate;
}

float BaseConnection::getReceiveRate() const
{
	return m_receiveRate;
}

void BaseConnection::setSendRate(float rate)
{
	m_sendRate = rate;
}

float BaseConnection::getSendRate() const
{
	return m_sendRate;
}