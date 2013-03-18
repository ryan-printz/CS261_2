#pragma once

#include "IProcessThread.h"
#include "windows.h"

class BaseProcessThread : public IProcessThread
{
public:
	BaseProcessThread() {};
	virtual ~BaseProcessThread() {};

protected:
	virtual void start(IProcess * process);
	virtual void stop();

	virtual void update() = 0;

	void *	m_thread;
	void *	m_event;
	bool	m_isStarted;
	CRITICAL_SECTION m_critical;
};

namespace ProcessThread
{
	unsigned long __stdcall Process(void * param);
};