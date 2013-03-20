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

	// pull the info from the connection manager.
	int ypos = 60;
	for(auto server = m_manager->begin(); server != m_manager->end(); ++server)
	{
		if(!(*server)->connected())
			continue;

		AEGfxPrint(40, ypos, 0xFFFFFFFF, (s8*)(*server)->info().c_str());
		//AEGfxPrint(100, ypos, 0xFFFFFFFF, game->m_ip);

		ypos += 30;
	}

	LeaveCriticalSection(&m_critical);
}
