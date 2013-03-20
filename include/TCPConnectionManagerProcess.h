#pragma once;

#include "BaseProcessThread.h"
#include "BaseProcess.h"

#include "ConnectionManager.h"
#include "TCPConnection.h"

class TCPConnectionManagerProcess : public BaseProcess
{
public:
	TCPConnectionManagerProcess(ConnectionManager<TCPConnection> * manager);
	virtual ~TCPConnectionManagerProcess() {};

	virtual void init();
	virtual void update();
	virtual void cleanup();

	ConnectionManager<TCPConnection> * m_manager;
};

class TCPConnectionManagerProcessThread : public BaseProcessThread
{
public:
	TCPConnectionManagerProcessThread(ConnectionManager<TCPConnection> * manager);
	virtual ~TCPConnectionManagerProcessThread();

	virtual void update();

	ConnectionManager<TCPConnection> * m_manager;
	IProcess * m_process;
};
