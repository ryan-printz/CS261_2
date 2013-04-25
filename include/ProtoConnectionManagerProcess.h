#pragma once

#include "BaseProcessThread.h"
#include "BaseProcess.h"

#include "ConnectionManager.h"
#include "ProtoConnection.h"

class ProtoConnectionManagerProcess : public BaseProcess
{
public:
	ProtoConnectionManagerProcess(ConnectionManager<ProtoConnection> * manager);
	virtual ~ProtoConnectionManagerProcess() {};

	virtual void init();
	virtual void update();
	virtual void cleanup();

	ConnectionManager<ProtoConnection> * m_manager;
};

class ProtoConnectionManagerProcessThread : public BaseProcessThread
{
public:
	ProtoConnectionManagerProcessThread(ConnectionManager<ProtoConnection> * manager);
	virtual ~ProtoConnectionManagerProcessThread();

	virtual void update();

	ConnectionManager<ProtoConnection> * m_manager;
	IProcess * m_process;
};
