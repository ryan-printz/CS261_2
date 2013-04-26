#pragma once

#include "BaseNetMessage.h"

struct NinjaInfoCardMessage : public BaseNetMessage
{
public:
	NinjaInfoCardMessage(short netID) 
		: BaseNetMessage(NINJA_INFO), m_netID(netID)
	{}

	const int & netID() const
	{
		return m_netID;
	}

protected:
	short m_netID;
};
