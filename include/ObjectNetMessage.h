#pragma once

#include "BaseNetMessage.h"

struct ObjectNetMessage : public BaseNetMessage
{
	ObjectNetMessage(short netId_, unsigned type_, unsigned flags_, char x_, char y_, char z_)
		: BaseNetMessage(OBJECT), netId(netId_), type(type_), flags(flags_), x(x_), y(y_), z(z_)
	{}

	short netId;
	unsigned type;
	unsigned flags;
	char x;
	char y;
	char z;
};
