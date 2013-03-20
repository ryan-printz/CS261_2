#pragma once

#include <string>

class ISocket;
class NetAddress;

class Packet
{
public:
	static const unsigned MAX = 1024;

	char m_buffer[MAX];
	int m_length;
};

class IConnection
{
public:
	virtual ~IConnection() {};

	virtual bool accept(ISocket * listener) = 0;

	virtual int send(Packet & p) = 0;
	virtual int receive(Packet & p) = 0;

	virtual void push_sendPacket(Packet & p) = 0;
	virtual void push_receivePacket(Packet & p) = 0;

	virtual bool pop_sendPacket(Packet & out) = 0;
	virtual bool pop_receivePacket(Packet & out) = 0;

	virtual const NetAddress & remote() const = 0;
	virtual std::string info() const = 0;

	virtual bool connected() const = 0;
};
