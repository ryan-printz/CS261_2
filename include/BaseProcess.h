#pragma once

#include "IProcess.h"

class BaseProcess : public IProcess
{
public:
	BaseProcess();
	virtual ~BaseProcess() {};

	virtual void init() = 0;
	virtual void update() = 0;
	virtual void cleanup() = 0;

	virtual void * getEvent() const;
	virtual void setEvent(void * e);

protected:
	void * m_event;
};