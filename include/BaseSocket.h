#pragma once

#include "ISocket.h"

class BaseSocket : public ISocket
{
public:
	BaseSocket();
	virtual ~BaseSocket() {};

	// virtual functions inherited from ISocket;
	virtual bool cleanup() = 0;

	virtual bool initialize(NetAddress address) = 0;

	virtual bool connect(const NetAddress & to) = 0;

	virtual bool listen(const NetAddress local, char backlog = 10) = 0;

	virtual ISocket * accept(NetAddress & remote) = 0;

	virtual int send(const char * buffer, unsigned size) = 0;
	virtual int send(const char * buffer, unsigned size, const NetAddress & to) = 0;

	virtual int receive(char * buffer, unsigned size) = 0;
	virtual int receive(char * buffer, unsigned size, NetAddress & from) = 0;

	// virtual functions implemented from ISocket;
	virtual bool invalid() const;
	virtual bool connected() const;
	virtual bool initialized() const;

	virtual bool blocking() const;
	virtual void setBlocking(bool blocking);

	virtual unsigned lastError() const;

protected:
	bool m_isInitialized;
	bool m_isBlocking;
	bool m_isConnected;
	unsigned int m_error;

	SOCKET m_socket;
};
