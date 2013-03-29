#pragma once

#include "BaseConnection.h"

class TCPConnection : public BaseConnection
{
public:
	TCPConnection();
	TCPConnection(ISocket * connection, NetAddress & address);
	virtual ~TCPConnection();

	virtual bool accept(ISocket * listener);

	virtual int send(Packet & p);
	virtual int receive(Packet & p);

	virtual std::string info() const;	
};
