#pragma once

class IProcess;

class IProcessThread
{
public:
	virtual	~IProcessThread() {};

	virtual void lock() = 0;
	virtual void unlock() = 0;

protected:
	virtual void start(IProcess *) = 0;
	virtual void update() = 0;
	virtual void stop() = 0;
};

