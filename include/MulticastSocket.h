#include "BaseSocket.h"
#include <Ws2ipdef.h>

class MulticastSocket : public BaseSocket
{
public:
	MulticastSocket();
	~MulticastSocket();

	virtual bool initialize(NetAddress address);
	virtual bool addToMulticastGroup(NetAddress member);
	virtual bool cleanup();
	virtual int send(const char * buffer, unsigned size, bool write = true);
	virtual int receive(char * buffer, unsigned size, bool write = true);
	virtual int receive(char * buffer, unsigned size, NetAddress & from, bool write = true);

protected:
	virtual bool connect(const NetAddress & to) { return false; }
	virtual bool listen(const NetAddress local, char backlog = 10) { return false; }
	virtual ISocket * accept(NetAddress & remote) { return nullptr; }
	virtual int send(const char * buffer, unsigned size, const NetAddress & to, bool write = true) { return 0; }
	
	ip_mreq m_multicast;
	NetAddress m_end;
	NetAddress m_recvd;
};