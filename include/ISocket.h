#pragma once

#include <WinSock2.h>

struct NetAddress : public sockaddr_in
{
	NetAddress(unsigned port);
	NetAddress(const char * ip, unsigned port);

	static char * localIP();

	const char * ip() const;
	unsigned port() const;
};

class ISocket
{
public:
	virtual ~ISocket() {};
	virtual bool cleanup() = 0;

	virtual bool initialize(NetAddress address) = 0;
	virtual bool initialized() const = 0;

	virtual bool blocking() const = 0;
	virtual void setBlocking(bool blocking) = 0;

	virtual bool connect() = 0;
	virtual bool connect(const NetAddress & to) = 0;
	virtual bool connected() const = 0;

	virtual bool listen(char backlog = 10) = 0;
	virtual bool invalid() const = 0;
	virtual unsigned lastError() const = 0;

	virtual int send(const char * buffer, unsigned size) = 0;
	virtual int send(const char * buffer, unsigned size, const NetAddress to) = 0;

	virtual int receive(const char * buffer, unsigned size) = 0;
	virtual int receive(const char * buffer, unsigned size, NetAddress & from) = 0;
};

bool initSockets(bool printInfo = false);
bool cleanSockets();
