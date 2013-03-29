#pragma once

enum MessageType
{
	CONNECT,
	DISCONNECT,
	SERVER_INFO,
	SERVER_LIST,
	SERVER_LIST_REQ
};

struct BaseNetMessage
{
public:
	BaseNetMessage(MessageType type) 
		: m_type(type) 
	{}

	MessageType type() const
	{
		return m_type;
	}

	template <typename MessageType>
	inline MessageType * as() 
	{
		return static_cast<MessageType*>(this);
	}

protected:
	MessageType m_type;
};