#pragma once

class IProcess
{
public:
	virtual ~IProcess() {};

	virtual void init() = 0;
	virtual void update() = 0;
	virtual void cleanup() = 0;

	virtual void * getEvent() const = 0;
	virtual void setEvent(void * event) = 0;
};
