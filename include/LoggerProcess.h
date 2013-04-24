#pragma once;

#include "BaseProcessThread.h"
#include "BaseProcess.h"


class LoggerProcess : public BaseProcess
{
public:
	LoggerProcess(/*ConnectionManager<TCPConnection> * manager*/);
	virtual ~LoggerProcess() {};

	virtual void init();
	virtual void update();
	virtual void cleanup();

	//ConnectionManager<TCPConnection> * m_manager;
};

class LoggerProcessThread : public BaseProcessThread
{
public:
	LoggerProcessThread(/*ConnectionManager<TCPConnection> * manager*/);
	virtual ~LoggerProcessThread();

	virtual void update();

	//ConnectionManager<TCPConnection> * m_manager;
	IProcess * m_process;
};
