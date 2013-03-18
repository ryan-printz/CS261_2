#pragma once

#include "BaseSocket.h"

class TCPSocket : public BaseSocket
{
public:
	TCPSocket();
	virtual ~TCPSocket();

	// virtual functions inherited from ISocket;
	virtual bool cleanup();
	virtual bool initialize(NetAddress address);

	virtual bool connect(const NetAddress & to);

	virtual ISocket * accept(NetAddress & remote);

	virtual bool listen(const NetAddress local, char backlog = 10);

	virtual int send(const char * buffer, unsigned size);
	virtual int send(const char * buffer, unsigned size, const NetAddress & to);

	virtual int receive(char * buffer, unsigned size);
	virtual int receive(char * buffer, unsigned size, NetAddress & from);

private:
	TCPSocket(SOCKET accepted);
};