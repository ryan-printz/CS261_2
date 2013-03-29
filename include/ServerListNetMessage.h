#pragma once

#include "BaseNetMessage.h"

#define ServerListReqNetMessage() BaseNetMessage(SERVER_LIST_REQ)

struct ServerListNetMessage : public BaseNetMessage
{
public:
	ServerListNetMessage(int count)
		: BaseNetMessage(SERVER_LIST), m_count(count)
	{}

	int count() const
	{ return m_count; }

protected:
	int m_count;
};
