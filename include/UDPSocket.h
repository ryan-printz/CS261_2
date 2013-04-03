#pragma once

#include "BaseSocket.h"

class UDPSocket : public BaseSocket
{
public:
	UDPSocket();
	virtual ~UDPSocket();

	virtual bool cleanup() = 0;

	virtual bool initialize(NetAddress address) = 0;

	virtual bool connect(const NetAddress & to) = 0;

	virtual bool listen(const NetAddress local, char backlog = 10) = 0;

	virtual ISocket * accept(NetAddress & remote) = 0;

	virtual int send(const char * buffer, unsigned size) = 0;
	virtual int send(const char * buffer, unsigned size, const NetAddress & to) = 0;

	virtual int receive(char * buffer, unsigned size) = 0;
	virtual int receive(char * buffer, unsigned size, NetAddress & from) = 0;
};
