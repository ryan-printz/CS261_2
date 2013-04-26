#pragma once

#include "BaseSocket.h"

class UDPSocket : public BaseSocket
{
public:
	UDPSocket();
	virtual ~UDPSocket();

	virtual bool cleanup();
	virtual bool initialize(NetAddress address);

	virtual int send(const char * buffer, unsigned size, bool write = true);
	virtual int send(const char * buffer, unsigned size, const NetAddress & to, bool write = true);

	virtual int receive(char * buffer, unsigned size, bool write = true);
	virtual int receive(char * buffer, unsigned size, NetAddress & from, bool write = true);

	const NetAddress & getSender() const;
	const NetAddress & getRecipient() const;

protected:
	virtual bool connect(const NetAddress &) { return false; }
	virtual bool listen(const NetAddress, char) { return false; }
	virtual ISocket * accept(NetAddress &) { return nullptr; }

	NetAddress m_from;
	NetAddress m_to;
};
