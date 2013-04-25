#pragma once

#include "BaseSocket.h"
#include "UDPHeader.h"
#include <list>

class UDPSocket : public BaseSocket
{
public:
	const static unsigned int MAX_PACKET_SIZE =	1024;

	struct Packet
	{
		NetAddress from;
		char	   packet[MAX_PACKET_SIZE];
		unsigned int	   packetSize;
	};

	UDPSocket();
	virtual ~UDPSocket();

	// virtual functions inherited from ISocket;
	virtual bool cleanup();
	virtual bool initialize(NetAddress address);

	bool connect();
	virtual bool connect(const NetAddress & to);

	virtual ISocket * accept(NetAddress & remote);
	

	virtual bool listen(const NetAddress local, char backlog = 10);

	virtual int send(const char * buffer, unsigned size, bool write = true);
	virtual int send(const char * buffer, unsigned size, const NetAddress & to);

	virtual int receive(char * buffer, unsigned size, bool write = true);
	virtual int receive(char * buffer, unsigned size, NetAddress & from);

	bool pop_receivePacket(char* buffer, int* size);

	void setUDPHeader(UDPHeader * header);
	UDPHeader getUDPHeader();

	UDPSocket acceptUDP(void);

private:
	UDPSocket(SOCKET accepted);

	void receiveSort();

	static std::list<NetAddress> m_connectionAttempts;
	static std::list<NetAddress> m_disconnected;
	static std::list<Packet>     m_received;

	unsigned int m_packetSize;
	NetAddress	m_address;
	UDPHeader * m_header;
	UDPHeader m_lastUDPHeader;
};