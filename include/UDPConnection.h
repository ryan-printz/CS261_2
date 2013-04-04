#pragma once

#include "BaseConnection.h"
#include "BaseSocket.h"
#include "SequenceNumber.h"
#include "UDPSocket.h"
#include <unordered_map>
#include <list>

struct PacketInfo
{
	SequenceNumber m_sequence;
	float		   m_time;
	uint		   m_size;
};

class LinkedOrderedQueue : public std::list<PacketInfo>
{
public:
	bool has(SequenceNumber no) const
	{
		for(const_iterator info = begin(); info != end(); ++info)
			if( info->m_sequence == no )
				return true;
		return false;
	}

	void advance(float dt)
	{
		for(iterator info = begin(); info != end(); ++info)
			info->m_time += dt;
	}

	void insert(const PacketInfo & pi)
	{
		if( empty() )
		{
			push_back(pi);
			return;
		}

		if( pi.m_sequence <= front().m_sequence )
			push_front(pi);
		else if( pi.m_sequence > back().m_sequence )
			push_back(pi);
		else
			for(iterator ipoint = begin(); ipoint != end(); ++ipoint)
				if( pi.m_sequence < ipoint->m_sequence )
				{
					std::list<PacketInfo>::insert(ipoint, pi);
					break;
				}
	}
};

struct ResendPacket
{
	SequenceNumber m_sequence;
	ubyte		   m_packet[256];
	float		   m_time;
	uint		   m_size;
};

class ResendOrderedQueue : public std::list<ResendPacket>
{
public:
	ResendOrderedQueue() : std::list<ResendPacket>()
	{}

	bool has(SequenceNumber no) const
	{
		for(const_iterator packet = begin(); packet != end(); ++packet)
			if( packet->m_sequence == no )
				return true;
		return false;
	}

	void advance(float dt)
	{
		for(iterator packet = begin(); packet != end(); ++packet)
			packet->m_time += dt;
	}

	void insert(const ResendPacket & rp)
	{
		if( empty() )
		{
			push_back(rp);
			return;
		}

		if( rp.m_sequence <= front().m_sequence )
			push_front(rp);
		else if( rp.m_sequence > back().m_sequence )
			push_back(rp);
		else
			for(iterator ipoint = begin(); ipoint != end(); ++ipoint)
				if( rp.m_sequence < ipoint->m_sequence )
				{
					std::list<ResendPacket>::insert(ipoint, rp);
					break;
				}
	}
};

struct FlowPacket
{
	ubyte	m_buffer[UDPSocket::MAX_PACKET_SIZE];
	ubyte	m_flags;
	uint	m_size;
};

bool operator==(const FlowPacket &lhs, const FlowPacket &rhs);
bool operator!=(const FlowPacket &lhs, const FlowPacket &rhs);
bool operator >(const FlowPacket &lhs, const FlowPacket &rhs);
bool operator <(const FlowPacket &lhs, const FlowPacket &rhs);
bool operator>=(const FlowPacket &lhs, const FlowPacket &rhs);
bool operator<=(const FlowPacket &lhs, const FlowPacket &rhs);

class LinkedPriorityQueue : public std::list<FlowPacket>
{
public:
	void insert(const FlowPacket & fp)
	{
		if( empty() )
			push_back(fp);

		if( fp <= front() )
			push_front(fp);
		else if( fp >= back() )
			push_back(fp);
		else
			for(iterator iplace = begin(); iplace != end(); ++iplace)
				if( fp < *iplace )
					std::list<FlowPacket>::insert(iplace, fp);
	};
};

struct ConnectionStats
{
	float m_upBandwidth;
	float m_downBandwith;
	float m_roundTripTime;

	int m_sentPackets;
	int m_receivedPackets;
	int m_lostPackets;
	int m_ackedPackets;
};

std::ostream & operator<<(std::ostream & os, const ConnectionStats & stats);

class UDPConnection : public BaseConnection
{
public:
	UDPConnection();
	UDPConnection(ISocket * connection, NetAddress & address);
	virtual ~UDPConnection();

	virtual bool accept(ISocket * listener);

	virtual int send(Packet & p);
	virtual int receive(Packet & p);

	virtual std::string info() const;	
};

