#pragma once

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

class IProcess
{
public:
	virtual ~IProcess() {};

	virtual void init() = 0;
	virtual void update() = 0;
	virtual void cleanup() = 0;

	virtual void * getEvent() const = 0;
	virtual void setEvent(void * event) = 0;

	virtual void setCritical(CRITICAL_SECTION * critical) = 0;
};
