#include "BaseProcess.h"

BaseProcess::BaseProcess()
{}

void BaseProcess::setEvent(void * e)
{
	m_event = e;
}

void * BaseProcess::getEvent() const
{
	return m_event;
}

void BaseProcess::setCritical(CRITICAL_SECTION * critical)
{
	m_critical = critical;
}