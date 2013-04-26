#pragma once

#include "BaseNetMessage.h"
#include "ClientInfo.h"

struct ClientInfoNetMessage : public BaseNetMessage
{
	ClientInfoNetMessage(ClientInfo & info)
		: BaseNetMessage(CLIENT_INFO)
	{
		strcpy(m_info.name, info.name);
		m_info.netID = info.netID;
	}

	int netID() const
	{
		return m_info.netID;
	}

	std::string name() const
	{
		return m_info.name;
	}

	ClientInfo m_info;
};
