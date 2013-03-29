#include "TCPConnectionManagerProcess.h"
#include "main.h"

TCPConnectionManagerProcess::TCPConnectionManagerProcess(ConnectionManager<TCPConnection> * manager)
	: m_manager(manager)
{}

void TCPConnectionManagerProcess::init()
{}

void TCPConnectionManagerProcess::update()
{
	EnterCriticalSection(m_critical);

	m_manager->update(0.016f);

	LeaveCriticalSection(m_critical);
}

void TCPConnectionManagerProcess::cleanup()
{
	delete m_manager;
}

TCPConnectionManagerProcessThread::TCPConnectionManagerProcessThread(ConnectionManager<TCPConnection> * manager)
	: m_manager(manager), m_process(nullptr)
{
	m_process = new TCPConnectionManagerProcess(manager);
	m_process->setEvent(m_event);
	m_process->setCritical(&m_critical);

	start( m_process );
}

TCPConnectionManagerProcessThread::~TCPConnectionManagerProcessThread()
{
	stop();

	delete m_process;
}

void TCPConnectionManagerProcessThread::update()
{
	EnterCriticalSection(&m_critical);
	LeaveCriticalSection(&m_critical);
}
