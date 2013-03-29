#pragma once

#include <iostream>
#include <BaseNetMessage.h>

template <typename ConnectionType>
ConnectionManager<ConnectionType>::ConnectionManager()
	: m_updateFrequencyReceive(0.016f),
	m_timerReceive(0.0f),
	m_updateFrequencySend(0.032f),
	m_timerSend(0.0f)
{
	push_back(new ConnectionType());
}

template <typename ConnectionType>
void ConnectionManager<ConnectionType>::setAcceptCallback(std::function<void(ConnectionType*)> callback)
{
	m_acceptCallback = callback;
}

template <typename ConnectionType>
ConnectionManager<ConnectionType>::~ConnectionManager()
{
	for(auto connection = begin(); connection != end(); ++connection)
		delete *connection;

	delete m_listener;
}

template <typename ConnectionType>
void ConnectionManager<ConnectionType>::setListener(ISocket * listener)
{
	if( !listener->invalid() && !listener->blocking() )
		m_listener = listener;
}

template <typename ConnectionType>
void ConnectionManager<ConnectionType>::updateTimers(float dt)
{
	m_timerReceive += dt;
	m_timerSend += dt;
}

template <typename ConnectionType>
void ConnectionManager<ConnectionType>::update(float dt)
{
	updateTimers(dt);

	if( m_timerReceive >= m_updateFrequencyReceive )
	{
		accept();
		receive();
		m_timerReceive -= m_updateFrequencyReceive;
	}

	if( m_timerSend >= m_updateFrequencySend )
	{
		send();
		m_timerSend -= m_updateFrequencySend;
	}
}

template <typename ConnectionType>
void ConnectionManager<ConnectionType>::accept()
{
	while( back()->accept(m_listener) )
	{
		m_acceptCallback( static_cast<ConnectionType*>(back()) );
		push_back(new ConnectionType());
	}
}

template <typename ConnectionType>
void ConnectionManager<ConnectionType>::send()
{
	Packet out;

	for(auto connection = begin(); connection != end(); ++connection)
		if( (*connection)->connected() && (*connection)->pop_sendPacket(out) )
			if(out.m_length != (*connection)->send(out))
				std::cerr << "Packet failed to send properly. Packet length: " 
						  << out.m_length << "Packet: " << out.m_buffer << std::endl;
}

template <typename ConnectionType>
void ConnectionManager<ConnectionType>::receive()
{
	Packet in;

	for(auto connection = begin(); connection != end(); ++connection)
		if ( (*connection)->connected() && 0 <= (*connection)->receive(in) )
		{
			if( in.m_length == 0)
				new (in.m_buffer) BaseNetMessage(DISCONNECT);

			(*connection)->push_receivePacket(in);
		}
}