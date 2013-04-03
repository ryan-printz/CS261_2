#pragma once

#include "IGameState.h"
#include "Asteroids.h"

class GameState_BasePlay : public IGameState
{
public:
	GameState_BasePlay();
	virtual ~GameState_BasePlay();

	virtual void load(void);
	virtual void init(void);
	virtual void update(void);
	virtual void draw(void);
	virtual void free(void);
	virtual void unload(void);

	virtual void onEnd() = 0;

	void loadGameObjList();

protected:
	Asteroids m_game;
};
