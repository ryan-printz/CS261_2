#include "ProtoConnectionManagerProcess.h"
#include "main.h"

ProtoConnectionManagerProcess::ProtoConnectionManagerProcess(ConnectionManager<ProtoConnection> * manager)
	: m_manager(manager)
{}

void ProtoConnectionManagerProcess::init()
{}

void ProtoConnectionManagerProcess::update()
{
	EnterCriticalSection(m_critical);

	m_manager->update(0.016f);

	LeaveCriticalSection(m_critical);
}

void ProtoConnectionManagerProcess::cleanup()
{
	delete m_manager;
}

ProtoConnectionManagerProcessThread::ProtoConnectionManagerProcessThread(ConnectionManager<ProtoConnection> * manager)
	: m_manager(manager), m_process(nullptr)
{
	m_process = new ProtoConnectionManagerProcess(manager);
	m_process->setEvent(m_event);
	m_process->setCritical(&m_critical);

	start( m_process );
}

ProtoConnectionManagerProcessThread::~ProtoConnectionManagerProcessThread()
{
	stop();

	delete m_process;
}

void ProtoConnectionManagerProcessThread::update()
{
	EnterCriticalSection(&m_critical);
	LeaveCriticalSection(&m_critical);
}
