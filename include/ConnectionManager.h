#pragma once

#include "ISocket.h"
#include "IConnection.h"

#include <list>

template <typename ConnectionType>
class ConnectionManager : public std::list<IConnection*>
{
public:
	ConnectionManager();
	~ConnectionManager();

	// given a currently listening, nonblocking socket.
	void setListener(ISocket * listener);

	void update(float dt);

private:
	void accept();
	void receive();
	void send();

	void updateTimers(float dt);

	ISocket * m_listener;

	float m_updateFrequencyReceive;
	float m_timerReceive;
	float m_updateFrequencySend;
	float m_timerSend;
};

#include "../src/ConnectionManager.inl"