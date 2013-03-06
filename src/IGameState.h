#pragma once

class GameStateManager;

class IGameState 
{
public:
	virtual ~IGameState() {};
	virtual void load(void) = 0;
	virtual void init(void) = 0;
	virtual void update(void) = 0;
	virtual void draw(void) = 0;
	virtual void free(void) = 0;
	virtual void unload(void) = 0;

protected:
	GameStateManager * m_gsm;
};
