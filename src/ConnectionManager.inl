#pragma once

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
		push_back( new ConnectionType() );
}

template <typename ConnectionType>
void ConnectionManager<ConnectionType>::send()
{
	Packet out;

	for(auto connection = begin(); connection != end(); ++connection)
		if( (*connection)->connected() && (*connection)->pop_sendPacket(out) )
			(*connection)->send(out);
}

template <typename ConnectionType>
void ConnectionManager<ConnectionType>::receive()
{
	Packet in;

	for(auto connection = begin(); connection != end(); ++connection)
		if( (*connection)->connected() && 0 < (*connection)->receive(in) )
			(*connection)->push_receivePacket(in);
}