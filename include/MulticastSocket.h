#pragma once

#include "UDPSocket.h"
#include <Ws2ipdef.h>

class MulticastSocket : public UDPSocket
{
public:
	MulticastSocket();
	virtual ~MulticastSocket();

	virtual bool initialize(NetAddress address);
	virtual bool addToMulticastGroup(NetAddress member);

protected:
	ip_mreq m_multicast;
};