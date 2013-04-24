#include "UDPConnection.h"
#include "UDPSocket.h"

#include <sstream>
#include <iostream>
#include <bitset>

UDPConnection::UDPConnection()
	: m_connected(false), m_useFlowControl(false), m_local(1)
{
	m_stats.m_ackedPackets = m_stats.m_lostPackets 
						   = m_stats.m_receivedPackets 
						   = m_stats.m_sentPackets = 0;
	m_stats.m_roundTripTime = m_stats.m_upBandwidth
							= m_stats.m_downBandwith = 0.0f;

	m_keepAliveInterval = 0;
	m_timeout = 2;

	m_idleTimer = 0.0f;
	m_modeTimer = 0.0f;
	m_unpenaltyTimer = 0.0f;

	m_penaltyTimer = 4.0f;
	m_sendRate = 0.01f;
	m_flowTimer = 0.f;
}

UDPConnection::UDPConnection(ISocket * connection, NetAddress & remote)
{
	m_socket = connection;
	m_remote = remote;
}

UDPConnection::~UDPConnection()
{
	delete m_socket;
}

bool UDPConnection::accept(ISocket * listener)
{
	UDPSocket* uSocket = dynamic_cast<UDPSocket*>(listener);
	
	if(!uSocket)
		return false;

	UDPSocket accepted = uSocket->acceptUDP();

	m_socket = listener->accept(m_remote);

	return m_socket != nullptr;
}

bool UDPConnection::connect(const char* ip, uint port)
{
	if(!m_connection.initialize(NetAddress(ip, port)))
		return false;
	if(!m_connection.connect())
		return false;
	m_connection.setBlocking(false);

	return true;
}

bool UDPConnection::cleanup()
{
	return false;
}

bool UDPConnection::disconnect()
{
	send((ubyte*)&UDPHeader::DISCONNECT_MESSAGE, sizeof(uint));
	return false;
}

int UDPConnection::send(Packet & p)
{
	return noFlowSend(p.m_buffer, p.m_length, UDPHeader::Flags::UDP_HIGH);
}

int UDPConnection::receive(Packet & p)
{
	return p.m_length = m_socket->receive(p.m_buffer, Packet::MAX);
}

int UDPConnection::receive(ubyte * buffer, uint len, int drop)
{
	ubyte packet[UDPSocket::MAX_PACKET_SIZE];
	uint headerSize = sizeof(UDPHeader);

	int received = m_connection.receive(packet, UDPSocket::MAX_PACKET_SIZE);

	// no packet was received
	// or the packet was not from this UDPcol.
	if( received == -1 )
		return -1;

	// Whoops, drop the packet.
	if( drop == -1 )
		return drop;

	// pull out the header.
	// adjust the received size appropriately.
	UDPHeader header = m_connection.getUDPHeader();

	std::cout << "Received Packet Header: Sequence number " << header.m_sequence.m_sequenceNumber
	<< ", Ack " << header.m_ack.m_sequenceNumber
	<< ", Acks " << std::bitset<32>((int)header.m_acks) 
	<< ", Flags " << std::bitset<CHAR_BIT>(header.m_flags) << std::endl;

	m_idleTimer = 0.0f;
	++m_stats.m_receivedPackets;

	// drop duplicate packets.
	if( m_receivedPackets.has( header.m_sequence ) )
		return -1;

	int resentShift = 0;
	if( header.m_flags & UDPHeader::UDP_RESENT )
	{
		SequenceNumber * old = reinterpret_cast<SequenceNumber*>(packet);
		received -= sizeof(SequenceNumber);

		PacketInfo resentInfo;
		resentInfo.m_sequence = *old;
		resentInfo.m_time	  = 0.0f;
		resentInfo.m_size	  = sizeof(SequenceNumber);

		m_receivedPackets.insert(resentInfo);
		resentShift = sizeof(SequenceNumber);
	}

	PacketInfo info;
	info.m_sequence	= header.m_sequence;
	info.m_time		= 0.0f;
	info.m_size		= received;

	// store the packet to generate acks later.
	m_receivedPackets.push_back( info );

	// update the remote sequence number.
	if( header.m_sequence > m_remoteHost )
		m_remoteHost = header.m_sequence;

	// check out what the packet ack'd
	useAck(header.m_ack, header.m_acks, header.m_flags & UDPHeader::UDP_RESENT);

	// don't return keep alive packets.
	// handle disconnect messages.
	if( received == sizeof(uint) && *(uint*)(packet) == UDPHeader::KEEP_ALIVE_MESSAGE )
		return -1;

	memcpy(buffer, packet + sizeof(uint) + resentShift, received -= sizeof(uint));

	if( header.m_flags & UDPHeader::UDP_HIGH )
		send((ubyte*)&UDPHeader::KEEP_ALIVE_MESSAGE, sizeof(uint), UDPHeader::UDP_NORMAL | UDPHeader::UDP_HIGH);

	return received;
}

int UDPConnection::send(ubyte * buffer, uint len, ubyte flags)
{
	if( !m_useFlowControl )
		return noFlowSend(buffer, len, flags);// ^ ProtoHeader::UDP_HIGH );

	FlowPacket fp;
	memcpy( fp.m_buffer, buffer, len );

	fp.m_size = len;
	fp.m_flags = flags;

	m_flowControl.insert(fp);

	return len;
}

void UDPConnection::update(float dt)
{
	// get a little bit closer to timing out.
	m_idleTimer += dt;

	if( m_keepAliveInterval && !((int)m_idleTimer % m_keepAliveInterval) )
	{
		send((ubyte*)&UDPHeader::KEEP_ALIVE_MESSAGE, sizeof(uint));
	}

	if( m_timeout && m_idleTimer > m_timeout )
	{
		m_connected = false;
	}

	// advance all the times in the packet queues.
	m_resend.advance(dt);
	m_sentPackets.advance(dt);
	m_ackedPackets.advance(dt);
	m_unackedPackets.advance(dt);
	m_receivedPackets.advance(dt);

	if( m_useFlowControl )
		updateFlowControl(dt);

	// update each set of packets.
	// remove all the sent packets that have been around for longer than timeout.
	while( !m_sentPackets.empty() && m_sentPackets.front().m_time > m_timeout )
		m_sentPackets.pop_front();

	// remove all the sent packets that have been around for longer than 2 * timeout
	// used to determine round trip time.
	while( !m_sentPackets.empty() && m_sentPackets.front().m_time > 2 * m_timeout )
		m_sentPackets.pop_front();

	// if a packet takes more than timeout time to be acked, it has been dropped.
	while( !m_unackedPackets.empty() && m_unackedPackets.front().m_time > m_timeout )
	{
		m_unackedPackets.pop_front();
		++m_stats.m_lostPackets;
	}

	if( !m_receivedPackets.empty() )
	{
		// maintain the last 33 packets received (by sequence number)
		const SequenceNumber latest = m_receivedPackets.back().m_sequence;
		const SequenceNumber oldest = (uint)latest >= 34 ? (latest - 34) : std::numeric_limits<uint>::max() - (34 - latest);

		// remove the received packets that have lower sequence numbers than the latest 33 packets.
		while( !m_receivedPackets.empty() && m_receivedPackets.front().m_sequence <= oldest )
			m_receivedPackets.pop_front();
	}

	// resend high priority packets if they still haven't been acked.
	for(auto resend = m_resend.begin(); resend != m_resend.end(); ++resend)
		if( resend->m_time > 1.5f * m_timeout )
		{
			send(resend->m_packet, resend->m_size, UDPHeader::UDP_RESENT);
			resend->m_time = 0.0f;
		}
}

std::string UDPConnection::info() const
{
	std::stringstream info;

	info << "UDP connection to ";
	info << m_remote.ip();
	info << " on ";
	info << m_remote.port();

	return info.str();
}

void UDPConnection::updateStats()
{
	int upBPS = 0;
	for(auto upPacket = m_sentPackets.begin(); upPacket != m_sentPackets.end(); ++upPacket)
		upBPS += upPacket->m_size;

	int downBPS = 0;
	for(auto downPacket = m_receivedPackets.begin(); downPacket != m_receivedPackets.end(); ++downPacket)
		downBPS += downPacket->m_size;

	// bytes per second
	upBPS /= m_timeout;
	downBPS /= m_timeout;

	// bits per millisecond.
	m_stats.m_upBandwidth = upBPS * 8 / 1000.0f;
	m_stats.m_downBandwith = downBPS * 8 / 1000.0f;
}

void UDPConnection::updateRTT(float time)
{
	m_stats.m_roundTripTime += (time - m_stats.m_roundTripTime) * 0.1f;
}

uint UDPConnection::makeAck()
{
	uint ack = 0;
	for(auto it = m_receivedPackets.begin(); it != m_receivedPackets.end(); ++it)
	{
		if( it->m_sequence == m_remoteHost || it->m_sequence > m_remoteHost )
			break;

		ubyte bitIndex = getBitIndex(it->m_sequence, m_remoteHost);
		if( bitIndex < 32 )
			ack |= 1 << bitIndex;
	}

	return ack;
}

void UDPConnection::useAck(SequenceNumber ack, uint ackPack, bool resent)
{
	if( m_unackedPackets.empty() )
		return;

	auto packet = m_unackedPackets.begin();
	while( packet != m_unackedPackets.end() )
	{
		bool packetAcked = false;
		
		if(packet->m_sequence == ack)
			packetAcked = true;
		else if( packet->m_sequence <= ack )
		{
			int bitIndex = getBitIndex(packet->m_sequence, ack);
			if( bitIndex < 32 )
				packetAcked = (ackPack >> bitIndex) & 1;
		}

		if( packetAcked )
		{
			// update stats.
			++m_stats.m_ackedPackets;
			updateRTT(packet->m_time);

			// remove the packet from the resend queue if it exists there.
			if( resent && m_resend.has(packet->m_sequence) )
				for(auto rp = m_resend.begin(); rp != m_resend.end(); ++rp)
				{
					if( rp->m_sequence == ack )
					{
						rp = m_resend.erase(rp);
						break;
					}
				}

			// move the packet to the acked queue
			// remove it from unacked.
			// set it as acked this update.
			m_ackedPackets.insert(*packet);
			m_acks.push_back(packet->m_sequence);
			packet = m_unackedPackets.erase( packet );
		}
		else
			++packet;
	}
}

ubyte UDPConnection::getBitIndex(const SequenceNumber & lhs, const SequenceNumber & rhs)
{
	if( lhs > rhs )
		return rhs + (std::numeric_limits<uint>::max() - lhs);
	else
		return rhs - 1 - lhs;
}

int UDPConnection::noFlowSend(ubyte * buffer, uint len, ubyte flags)
{
	UDPHeader header;
	header.m_sequence = m_local;
	header.m_ack	  = m_remoteHost;
	header.m_acks	  = makeAck();
	header.m_flags	  = flags;

	m_connection.setUDPHeader(&header);

	int sent = 0;

	if((sent = m_connection.send(buffer, len)) != len)
		return -1;

	std::cout << "Sent Packet Header: Sequence number " << header.m_sequence.m_sequenceNumber
	//<< ", Ack " << header.m_ack.m_sequenceNumber
	//<< ", Acks " << std::bitset<32>((int)header.m_acks) 
	<< ", Flags " << std::bitset<CHAR_BIT>(header.m_flags) << std::endl;
	
	if( flags & UDPHeader::UDP_HIGH )
	{
		ResendPacket repack;

		memcpy( repack.m_packet, &header.m_sequence, sizeof(SequenceNumber) );
		memcpy( repack.m_packet + sizeof(SequenceNumber), buffer, len );

		repack.m_size = len + sizeof(SequenceNumber);
		repack.m_sequence = header.m_sequence;
		repack.m_time = 0.0f;

		m_resend.insert( repack );
	}

	// store info about this packet for stats.
	PacketInfo info;
	info.m_sequence = m_local;
	info.m_time		= 0.0f;
	info.m_size		= len;

	m_sentPackets.push_back(info);
	m_unackedPackets.push_back(info);

	++m_local;
	++m_stats.m_sentPackets;

	return sent;
}

void UDPConnection::updateFlowControl(float dt)
{
	m_flowTimer += dt;

	// if send rate > 10fps
	if( m_sendRate < 0.01f )
	{
		if( m_stats.m_roundTripTime > m_goodRoundTripTime )
		{
			// degrade the connection.
			if( (m_sendRate *= 2.0f) > 0.01f )
				m_sendRate = 0.01f;

			if( m_modeTimer > 10.0f && m_penaltyTimer < 60.0f )
				if( (m_penaltyTimer *= 2.0f) > 60.0f )
					m_penaltyTimer = 60.0f;

			m_modeTimer = 0.0f;
			m_unpenaltyTimer = 0.0f;
		}
		else
		{
			m_modeTimer += dt;
			if( (m_unpenaltyTimer += dt) > 10.0f && m_penaltyTimer > 1.0f )
			{
				if( (m_penaltyTimer *= 0.5f) < 1.0f )
					m_penaltyTimer = 1.0f;
				m_unpenaltyTimer = 0.0f;
			}
		}
	}
		
	// if send rate < 30fps
	if( m_sendRate > 0.0032f )
	{
		if( m_stats.m_roundTripTime <= m_goodRoundTripTime )
			m_modeTimer += dt;
		else
			m_modeTimer = 0.0f;

		if( m_modeTimer > m_penaltyTimer )
		{
			m_modeTimer = 0.0f;
			m_penaltyTimer = 0.0f;

			if( (m_sendRate *= 0.5f) < 0.0032f )
				m_sendRate = 0.0032f;
		}
	}

	while( m_flowTimer > (1.0f / m_sendRate) / 10000.f && !m_flowControl.empty() )
	{
		noFlowSend(m_flowControl.back().m_buffer, m_flowControl.back().m_size, m_flowControl.back().m_flags);
		m_flowControl.pop_back();

		m_flowTimer -= (1.0f/m_sendRate) / 10000.f;
	}
}

std::ostream & operator<<(std::ostream & os, const ConnectionStats & stats)
{
	std::ostream::sentry ok(os);
	os << "up/down bandwidth: " << stats.m_upBandwidth << "/" << stats.m_downBandwith << std::endl;
	os << "ping: " << stats.m_roundTripTime << std::endl; 
	os << "lifetime sent: " << stats.m_sentPackets << std::endl;
	os << "lifetime acked: " << stats.m_ackedPackets << std::endl;
	os << "lifetime received: " << stats.m_receivedPackets << std::endl; 
	os << "lifetime lost: " << stats.m_lostPackets;
	return os;
}

bool operator==(const FlowPacket &lhs, const FlowPacket &rhs)
{
	const ubyte highlownorm = UDPHeader::UDP_HIGH |
							  UDPHeader::UDP_LOW | 
							  UDPHeader::UDP_NORMAL;
	ubyte priority = lhs.m_flags & rhs.m_flags;
	return priority & highlownorm ? true : false;
}

bool operator!=(const FlowPacket &lhs, const FlowPacket &rhs)
{
	return !operator==(lhs, rhs);
}

bool operator >(const FlowPacket &lhs, const FlowPacket &rhs)
{
	// lhs has greater priority iff:
	// lhs is high priority && rhs is NOT high priority.
	// lhs is NOT low priority && rhs is low priority.
	return ((lhs.m_flags & UDPHeader::UDP_HIGH) && !(rhs.m_flags & UDPHeader::UDP_HIGH))
		|| (!(lhs.m_flags & UDPHeader::UDP_LOW) && (rhs.m_flags & UDPHeader::UDP_LOW));
}

bool operator <(const FlowPacket &lhs, const FlowPacket &rhs)
{
	return operator>(rhs, lhs);
}

bool operator>=(const FlowPacket &lhs, const FlowPacket &rhs)
{
	return !operator<(lhs, rhs);
}

bool operator<=(const FlowPacket &lhs, const FlowPacket &rhs)
{
	return !operator>(lhs, rhs);
}
