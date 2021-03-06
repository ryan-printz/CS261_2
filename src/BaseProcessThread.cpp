#include "BaseProcessThread.h"
#include "IProcess.h"

#include <Windows.h>
#define WIN32_LEAN_AND_MEAN

BaseProcessThread::BaseProcessThread()
	: m_event(nullptr), m_thread(nullptr), m_isStarted(false)
{}

void BaseProcessThread::start(IProcess * process) {
    if (m_isStarted)
        return;

	InitializeCriticalSection(&m_critical);

    m_event  = CreateEvent(NULL, true, false, TEXT("ExitEvent"));
	m_thread = CreateThread(nullptr, 0, ProcessThread::Process, process, 0, nullptr);

	if( m_thread )
		m_isStarted = true;
}

void BaseProcessThread::stop() 
{
    if (!m_isStarted)
        return;

    SetEvent( m_event );
    CloseHandle( m_thread );
	CloseHandle( m_event );
    DeleteCriticalSection( &m_critical );

	m_isStarted = false;
}

void BaseProcessThread::lock()
{
	EnterCriticalSection(&m_critical);
}

void BaseProcessThread::unlock()
{
	LeaveCriticalSection(&m_critical);
}

unsigned long __stdcall ProcessThread::Process( void * param ) 
{
	IProcess * process = reinterpret_cast<IProcess*>(param);
	process->init();

    while (true)
	{
		process->update();

        DWORD result = WaitForSingleObject( process->getEvent(), 0 );

        switch ( result ) 
		{
        case WAIT_TIMEOUT: 
            break;

		case WAIT_OBJECT_0:
			process->cleanup();
            return 0;
        }
    }
}