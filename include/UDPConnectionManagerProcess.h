#pragma once;

#include "BaseProcessThread.h"
#include "BaseProcess.h"

#include "ConnectionManager.h"
#include "UDPConnection.h"

class UDPConnectionManagerProcess : public BaseProcess
{
public:
	UDPConnectionManagerProcess(ConnectionManager<UDPConnection> * manager);
	virtual ~UDPConnectionManagerProcess() {};

	virtual void init();
	virtual void update();
	virtual void cleanup();

	ConnectionManager<UDPConnection> * m_manager;
};

class UDPConnectionManagerProcessThread : public BaseProcessThread
{
public:
	UDPConnectionManagerProcessThread(ConnectionManager<UDPConnection> * manager);
	virtual ~UDPConnectionManagerProcessThread();

	virtual void update();

	ConnectionManager<UDPConnection> * m_manager;
	IProcess * m_process;
};
