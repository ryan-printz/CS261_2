#include "ProtoSocket.h"

std::list<NetAddress>				ProtoSocket::m_connectionAttempts;
std::list<NetAddress>				ProtoSocket::m_disconnected;
std::list<ProtoSocket::Packet>		ProtoSocket::m_received;

ProtoSocket::ProtoSocket()
{}

ProtoSocket::ProtoSocket(SOCKET accepted)
{
	m_isConnected = m_isInitialized = true;
	m_socket = accepted;
}

ProtoSocket::~ProtoSocket()
{}

bool ProtoSocket::cleanup()
{
	if( m_isInitialized && shutdown(m_socket, SD_BOTH) )
	{
		m_isConnected =	m_isInitialized = m_isBlocking = false;
		return false;
	}

	if( closesocket(m_socket) )
		return false;

	return true;
}

bool ProtoSocket::initialize(NetAddress address)
{
	m_address = address;
	//Probably not what should happen?
	m_socket = socket(address.sin_family, SOCK_DGRAM, IPPROTO_UDP);

	::bind(m_socket, (SOCKADDR*)&m_address, sizeof(m_address));
	return m_isInitialized = (m_socket != INVALID_SOCKET);
}
bool ProtoSocket::connect()
{
	return connect(m_address);
}
bool ProtoSocket::connect(const NetAddress & to)
{

	ubyte buffer[sizeof(ProtoHeader) + sizeof(uint)];

	ProtoHeader temp;
	
	memcpy(buffer, &temp, sizeof(ProtoHeader));
	memcpy(buffer + sizeof(ProtoHeader), (ubyte*)&ProtoHeader::CONNECTION_MESSAGE, sizeof(uint));

	bool test = ((sizeof(uint) + sizeof(ProtoHeader)) == ::sendto(m_socket, (const char*)buffer, sizeof(uint) + sizeof(ProtoHeader), 0, (SOCKADDR*)&to, sizeof(NetAddress)));
	return m_isConnected = test;

	/*if( !m_isInitialized || ::connect(m_socket, (SOCKADDR*)&to, sizeof(to)) )
		return false;

	return m_isConnected = true;*/
}

bool ProtoSocket::listen(const NetAddress local, char backlog) 
{
	do 
	{ receiveSort(); }
	while(m_isBlocking && m_connectionAttempts.empty());

	while( m_connectionAttempts.size() > backlog )
		m_connectionAttempts.pop_front();

	return true;
}

ISocket * ProtoSocket::accept(NetAddress & remote) 
{
	SOCKET accepted = INVALID_SOCKET;
	ISocket * newSocket = nullptr;
	int addrlen = sizeof(remote);

	accepted = ::accept(m_socket, (SOCKADDR*)&remote, &addrlen);
	if(accepted != INVALID_SOCKET)
		newSocket = new ProtoSocket(accepted);

	return newSocket;
}

ProtoSocket ProtoSocket::acceptUDP()
{
	ProtoSocket accepted;

	receiveSort();

	if( m_connectionAttempts.empty() )
		return accepted;

	accepted.m_socket = m_socket;
	accepted.m_address = m_connectionAttempts.front();
	accepted.m_isConnected = true;
	accepted.m_isBlocking = m_isBlocking;

	m_connectionAttempts.pop_front();

	return accepted;
}

int ProtoSocket::send(const char * buffer, unsigned size, bool write)
{
	int i = send(buffer, size, m_address);
	if(write)
	{
		FILE* myFile;
		myFile = fopen ("log.txt", "a");
		fwrite("Sending UDP: ", 1, 13, myFile);
		fwrite(buffer, 1, size, myFile);
		fwrite("\n", 1, 1, myFile);
		fclose(myFile);
	}	
	return i;
}

int ProtoSocket::send(const char * buffer, unsigned size, const NetAddress &to, bool write)
{
	ubyte packet[MAX_PACKET_SIZE];

	memcpy(packet, m_header, sizeof(ProtoHeader));
	memcpy(packet + sizeof(ProtoHeader), buffer, std::min(size, MAX_PACKET_SIZE - sizeof(ProtoHeader)));
	return ::sendto(m_socket, packet, std::min(size+sizeof(ProtoHeader), MAX_PACKET_SIZE), 0, (SOCKADDR*)&to, sizeof(NetAddress)) - sizeof(ProtoHeader);
}

int ProtoSocket::receive(char * buffer, unsigned size, bool write)
{
	//return ::recv(m_socket, buffer, size, 0);
	std::list<Packet>::iterator packet;
		
	do {
		receiveSort();

		packet = m_received.begin();
		for( ; packet != m_received.end(); ++packet  )
			if( packet->from.sin_port == m_address.sin_port 
				&& packet->from.sin_addr.s_addr == m_address.sin_addr.s_addr )
				break;
	} while (m_isBlocking && packet == m_received.end());

	if( packet == m_received.end() )
		return -1;

	uint len = std::min(size, packet->packetSize);
	memcpy(&m_lastProtoHeader, packet->packet, sizeof(ProtoHeader));
	memcpy(buffer, packet->packet, len);

	m_received.erase(packet);

	return len;
}

int ProtoSocket::receive(char * buffer, unsigned size, NetAddress &from, bool write)
{
	int rcvd = receive(buffer, size);
	if(rcvd > 0)
		from = m_address;
	return rcvd;
}

void ProtoSocket::receiveSort()
{
	Packet p;
	int addrSize = sizeof(SOCKADDR);

	p.packetSize = ::recvfrom(m_socket, p.packet, sizeof(p.packet), 0, (SOCKADDR*)&p.from, &addrSize);

	m_error = WSAGetLastError();
	// nothing to receive


	if( p.packetSize == SOCKET_ERROR && m_error == WSAEWOULDBLOCK )
		return;
	if( p.packetSize > 0 && p.packetSize != SOCKET_ERROR)
	{
		FILE* myFile;
		myFile = fopen ("log.txt", "a");
		fwrite("Received UDP: ", 1, 14, myFile);
		fwrite(p.packet, 1, p.packetSize, myFile);
		fwrite("\n", 1, 1, myFile);
		fclose(myFile);
	}
	// client disconnected.
	if( p.packetSize == 0 )
	{
		m_disconnected.emplace_back( p.from );
		return;
	}

	// not from a proto send
	else if( p.packetSize < sizeof(ProtoHeader) || !reinterpret_cast<ProtoHeader*>(p.packet)->valid() )
		return;

	// connection attempt
	else if ( p.packetSize - sizeof(ProtoHeader) == sizeof(uint) 
			&& ProtoHeader::CONNECTION_MESSAGE == *((uint*)(p.packet + sizeof(ProtoHeader))) )
	{
		printf("Connection attempt!\n");
		m_connectionAttempts.emplace_back(p.from);
		return;
	}

	// disconnection
	else if ( p.packetSize - sizeof(ProtoHeader) == sizeof(uint) 
			&& ProtoHeader::DISCONNECT_MESSAGE == *((uint*)(p.packet + sizeof(ProtoHeader))) )
	{
		m_disconnected.emplace_back(p.from);
		return;
	}
	//memmove(p.packet, p.packet + sizeof(ProtoHeader), sizeof(ProtoHeader));
	//p.packetSize -= sizeof(ProtoHeader);
	// else it's just a packet.
	m_received.emplace_back(p);
}

void ProtoSocket::setProtoHeader(ProtoHeader * header)
{
	m_header = header;
}

ProtoHeader ProtoSocket::getProtoHeader()
{
	return m_lastProtoHeader;
}

bool ProtoSocket::pop_receivePacket(char* buffer, int* size)
{
	if( m_received.empty() )
		return false;

	Packet out = m_received.front();
	m_received.pop_front();

	memcpy(buffer, out.packet, out.packetSize);
	*size = out.packetSize;
	return true;
}