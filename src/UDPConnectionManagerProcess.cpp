#include "UDPConnectionManagerProcess.h"
#include "main.h"

UDPConnectionManagerProcess::UDPConnectionManagerProcess(ConnectionManager<UDPConnection> * manager)
	: m_manager(manager)
{}

void UDPConnectionManagerProcess::init()
{}

void UDPConnectionManagerProcess::update()
{
	EnterCriticalSection(m_critical);

	m_manager->update(0.016f);

	LeaveCriticalSection(m_critical);
}

void UDPConnectionManagerProcess::cleanup()
{
	delete m_manager;
}

UDPConnectionManagerProcessThread::UDPConnectionManagerProcessThread(ConnectionManager<UDPConnection> * manager)
	: m_manager(manager), m_process(nullptr)
{
	m_process = new UDPConnectionManagerProcess(manager);
	m_process->setEvent(m_event);
	m_process->setCritical(&m_critical);

	start( m_process );
}

UDPConnectionManagerProcessThread::~UDPConnectionManagerProcessThread()
{
	stop();

	delete m_process;
}

void UDPConnectionManagerProcessThread::update()
{
	EnterCriticalSection(&m_critical);
	LeaveCriticalSection(&m_critical);
}
