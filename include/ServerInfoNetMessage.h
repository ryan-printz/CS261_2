#pragma once

#include "ServerInfo.h"
#include "BaseNetMessage.h"

struct ServerInfoNetMessage : public BaseNetMessage
{
public:
	ServerInfoNetMessage(ServerInfo & info) 
		: BaseNetMessage(SERVER_INFO), m_info(info)
	{}

	const ServerInfo & info() const
	{
		return m_info;
	}

protected:
	ServerInfo m_info;
};
