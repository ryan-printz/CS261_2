#include "UDPSocket.h"

std::list<NetAddress>				UDPSocket::m_connectionAttempts;
std::list<NetAddress>				UDPSocket::m_disconnected;
std::list<UDPSocket::Packet>		UDPSocket::m_received;

UDPSocket::UDPSocket()
{}

UDPSocket::UDPSocket(SOCKET accepted)
{
	m_isConnected = m_isInitialized = true;
	m_socket = accepted;
}

UDPSocket::~UDPSocket()
{}

bool UDPSocket::cleanup()
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

bool UDPSocket::initialize(NetAddress address)
{
	m_socket = socket(address.sin_family, SOCK_STREAM, IPPROTO_UDP);

	return m_isInitialized = (m_socket != INVALID_SOCKET);
}

bool UDPSocket::connect(const NetAddress & to)
{
	if( !m_isInitialized || ::connect(m_socket, (SOCKADDR*)&to, sizeof(to)) )
		return false;

	return m_isConnected = true;
}

bool UDPSocket::listen(const NetAddress local, char backlog) 
{
	if ( ::bind(m_socket, (SOCKADDR*)&local, sizeof(local)) )
		return false;

	if ( ::listen(m_socket, backlog) )
		return false;

	return true;
}

ISocket * UDPSocket::accept(NetAddress & remote) 
{
	SOCKET accepted = INVALID_SOCKET;
	ISocket * newSocket = nullptr;
	int addrlen = sizeof(remote);

	accepted = ::accept(m_socket, (SOCKADDR*)&remote, &addrlen);
	if(accepted != INVALID_SOCKET)
		newSocket = new UDPSocket(accepted);

	return newSocket;
}

UDPSocket UDPSocket::acceptUDP()
{
	UDPSocket accepted;

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

int UDPSocket::send(const char * buffer, unsigned size)
{
	return ::send(m_socket, buffer, size, 0);
}

int UDPSocket::send(const char * buffer, unsigned size, const NetAddress &)
{
	return send(buffer, size);
}

int UDPSocket::receive(char * buffer, unsigned size)
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

	uint len = std::min(size, packet->packetSize - sizeof(UDPHeader));
	memcpy(&m_lastUDPHeader, packet->packet, sizeof(UDPHeader));
	memcpy(buffer, packet->packet + sizeof(UDPHeader), len);

	m_received.erase(packet);

	return size;
}

int UDPSocket::receive(char * buffer, unsigned size, NetAddress &)
{
	return receive(buffer, size);
}

void UDPSocket::receiveSort()
{
	Packet p;

	p.packetSize = receive(p.packet, m_packetSize, p.from);

	// nothing to receive
	if( p.packetSize == SOCKET_ERROR && m_error == WSAEWOULDBLOCK )
		return;
	
	// client disconnected.
	else if( p.packetSize == 0 )
	{
		m_disconnected.emplace_back( p.from );
		return;
	}

	// not from a proto send
	else if( p.packetSize < sizeof(UDPHeader) || !reinterpret_cast<UDPHeader*>(p.packet)->valid() )
		return;

	// connection attempt
	else if ( p.packetSize - sizeof(UDPHeader) == sizeof(uint) 
			&& UDPHeader::CONNECTION_MESSAGE == *((uint*)(p.packet + sizeof(UDPHeader))) )
	{
		m_connectionAttempts.emplace_back(p.from);
		return;
	}

	// disconnection
	else if ( p.packetSize - sizeof(UDPHeader) == sizeof(uint) 
			&& UDPHeader::DISCONNECT_MESSAGE == *((uint*)(p.packet + sizeof(UDPHeader))) )
	{
		m_disconnected.emplace_back(p.from);
		return;
	}
		
	// else it's just a packet.
	m_received.emplace_back(p);
}
