#pragma once

#include <iostream>
#include <BaseNetMessage.h>

template <typename ConnectionType>
ConnectionManager<ConnectionType>::ConnectionManager()
	: m_updateFrequencyReceive(0.016f),
	m_updateFrequencySend(0.032f),
	m_timerAccept(0.0f)
{
	auto newConnection = new ConnectionType();

	newConnection->setSendRate(m_updateFrequencySend);
	newConnection->setReceiveRate(m_updateFrequencyReceive);

	push_back(newConnection);
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
void ConnectionManager<ConnectionType>::update(float dt)
{
	if( (m_timerAccept += dt) >= m_updateFrequencyReceive )
	{
		accept();
		m_timerAccept -= m_updateFrequencyReceive;
	}

	for(auto connection = begin(); connection != end(); ++connection)
		if( (*connection)->connected() )
			(*connection)->update(dt);
}

template <typename ConnectionType>
void ConnectionManager<ConnectionType>::accept()
{
	while( back()->accept(m_listener) )
	{
		m_acceptCallback( static_cast<ConnectionType*>(back()) );
		
		auto newConnection = new ConnectionType();

		newConnection->setReceiveRate(m_updateFrequencyReceive);
		newConnection->setSendRate(m_updateFrequencySend);

		push_back(newConnection);
	}
}