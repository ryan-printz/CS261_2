#pragma once

#include "ISocket.h"
#include "IConnection.h"

#include <list>
#include <functional>

template <typename ConnectionType>
class ConnectionManager : public std::list<IConnection*>
{
public:
	ConnectionManager();
	~ConnectionManager();

	// given a currently listening, nonblocking socket.
	void setListener(ISocket * listener);
	void setAcceptCallback(std::function<void(ConnectionType*)> callback);

	void update(float dt);

private:
	void accept();

	ISocket * m_listener;

	float m_updateFrequencyReceive;
	float m_updateFrequencySend;
	float m_timerAccept;

	std::function<void(ConnectionType*)> m_acceptCallback;
};

#include "../src/ConnectionManager.inl"