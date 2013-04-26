#include "MulticastSocket.h"
#include <iostream>

MulticastSocket::MulticastSocket()
	: UDPSocket()
{}

MulticastSocket::~MulticastSocket()
{}

bool MulticastSocket::addToMulticastGroup(NetAddress member)
{
	if( ::setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)member.ip(), sizeof(NetAddress)) )
		return false;
}

bool MulticastSocket::initialize(NetAddress address)
{
	if( !UDPSocket::initialize(address) )
		return false;

	NetAddress local(NetAddress::localIP(), address.port());
	if( ::bind(m_socket, (SOCKADDR*)&local, sizeof(NetAddress)) )
		return false;
	int err = WSAGetLastError();

	// set multicast address
	m_multicast.imr_multiaddr.s_addr = inet_addr(address.ip());
	
	m_to.sin_addr = m_multicast.imr_multiaddr;
	m_to.sin_family = AF_INET;
	m_to.sin_port = address.sin_port;

	m_multicast.imr_interface.s_addr = inet_addr(NetAddress::localIP());
	err = ::setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&m_multicast, sizeof(m_multicast)); 
	int x = WSAGetLastError();
	printf("ret: %d, lasterror = %d, %s\n", err, x, inet_ntoa(m_multicast.imr_interface));

	int ttl = 1;
	::setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&ttl, sizeof(ttl));

	int loopback = 0;
	::setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_LOOP, (const char*)&loopback, sizeof(loopback));
	err = WSAGetLastError();
	return m_isInitialized = (m_socket != INVALID_SOCKET);
}
