#pragma once

#include "IConnection.h"
#include "ISocket.h"
#include <list>

class BaseConnection : public IConnection
{
public:
	BaseConnection();
	virtual ~BaseConnection() {};

	virtual bool accept(ISocket * listener) = 0;

	virtual int send(Packet & p) = 0;
	virtual int receive(Packet & p) = 0;

	virtual const char * info() const = 0;

	// members implemented from IConnection
	virtual bool connected() const;
	virtual const NetAddress & remote() const;

	virtual void push_sendPacket(Packet & p);
	virtual void push_receivePacket(Packet & p);

	virtual bool pop_sendPacket(Packet & out);
	virtual bool pop_receivePacket(Packet & out);

protected:
	ISocket * m_socket;
	NetAddress m_remote;
	
	std::list <Packet> m_send;
	std::list <Packet> m_receive;
};
