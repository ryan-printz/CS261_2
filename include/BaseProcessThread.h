#pragma once

#include "IProcessThread.h"
#include "IProcess.h"

class BaseProcessThread : public IProcessThread
{
public:
	BaseProcessThread();
	virtual ~BaseProcessThread() {};

	virtual void lock();
	virtual void unlock();

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