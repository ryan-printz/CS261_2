#pragma once

#include "BaseConnection.h"

class UDPConnection : public BaseConnection
{
public:
	UDPConnection();
	UDPConnection(ISocket * connection, NetAddress & address);
	virtual ~UDPConnection();

	virtual bool accept(ISocket * listener);

	virtual int send(Packet & p);
	virtual int receive(Packet & p);

	virtual std::string info() const;	
};
