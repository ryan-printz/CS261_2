#pragma once

#include "BaseSocket.h"
#include "ProtoHeader.h"
#include <list>

class ProtoSocket : public BaseSocket
{
public:
	const static unsigned int MAX_PACKET_SIZE =	1024;

	struct Packet
	{
		NetAddress from;
		char	   packet[MAX_PACKET_SIZE];
		unsigned int	   packetSize;
	};

	ProtoSocket();
	virtual ~ProtoSocket();

	// virtual functions inherited from ISocket;
	virtual bool cleanup();
	virtual bool initialize(NetAddress address);

	bool connect();
	virtual bool connect(const NetAddress & to);

	virtual ISocket * accept(NetAddress & remote);
	

	virtual bool listen(const NetAddress local, char backlog = 10);

	virtual int send(const char * buffer, unsigned size, bool write = true);
	virtual int send(const char * buffer, unsigned size, const NetAddress & to, bool write = true);

	virtual int receive(char * buffer, unsigned size, bool write = true);
	virtual int receive(char * buffer, unsigned size, NetAddress & from, bool write = true);

	bool pop_receivePacket(char* buffer, int* size);

	void setProtoHeader(ProtoHeader * header);
	ProtoHeader getProtoHeader();

	ProtoSocket acceptUDP(void);

private:
	ProtoSocket(SOCKET accepted);

	void receiveSort();

	static std::list<NetAddress> m_connectionAttempts;
	static std::list<NetAddress> m_disconnected;
	static std::list<Packet>     m_received;

	unsigned int m_packetSize;
	NetAddress	m_address;
	ProtoHeader * m_header;
	ProtoHeader m_lastProtoHeader;
};