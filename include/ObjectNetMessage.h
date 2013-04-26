#pragma once

#include "BaseNetMessage.h"

struct ObjectNetMessage : public BaseNetMessage
{
	ObjectNetMessage(short netId_, unsigned type_, unsigned flags_, float x_, float y_, float z_, float velx_, float vely_)
		: BaseNetMessage(OBJECT), netId(netId_), type(type_), flags(flags_), x(x_), y(y_), z(z_), velx(velx_), vely(vely_)
	{}

	short netId;
	unsigned type;
	unsigned flags;
	float x;
	float y;
	float z;
	float velx;
	float vely;
};
