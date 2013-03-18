#pragma once

class IProcess;

class IProcessThread
{
public:
	virtual	~IProcessThread() {};

protected:
	virtual void start(IProcess *) = 0;
	virtual void update() = 0;
	virtual void stop() = 0;
};

