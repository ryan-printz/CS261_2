#include "LoggerProcess.h"
#include "main.h"

LoggerProcess::LoggerProcess(/*ConnectionManager<TCPConnection> * manager*/)
	/*: m_manager(manager)*/
{}

void LoggerProcess::init()
{}

void LoggerProcess::update()
{
	EnterCriticalSection(m_critical);

	m_manager->update(0.016f);

	LeaveCriticalSection(m_critical);
}

void LoggerProcess::cleanup()
{
	delete m_manager;
}

LoggerProcessThread::LoggerProcessThread(/*ConnectionManager<TCPConnection> * manager*/)
	: /*m_manager(manager),*/ m_process(nullptr)
{
	m_process = new LoggerProcess(/*manager*/);
	m_process->setEvent(m_event);
	m_process->setCritical(&m_critical);

	start( m_process );
}

LoggerProcessThread::~LoggerProcessThread()
{
	stop();

	delete m_process;
}

void LoggerProcessThread::update()
{
	EnterCriticalSection(&m_critical);
	LeaveCriticalSection(&m_critical);
}
