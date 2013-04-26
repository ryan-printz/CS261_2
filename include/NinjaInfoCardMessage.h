#pragma once

#include "BaseNetMessage.h"

struct NinjaInfoCardMessage : public BaseNetMessage
{
public:
	NinjaInfoCardMessage(int & netID) 
		: BaseNetMessage(NINJA_INFO), m_netID(netID)
	{}

	const int & netID() const
	{
		return m_netID;
	}

protected:
	int m_netID;
};
