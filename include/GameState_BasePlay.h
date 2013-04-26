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

	void updateInput();
	void updateAsteroids();
	void updatePhysics();
	void updateObjects();
	void checkCollision();
	void updateMatrix();

	bool playerCollide(GameObjInst * me, GameObjInst * other);	
	bool bulletCollide(GameObjInst * me, GameObjInst * other);
	bool bombCollide(GameObjInst * me, float radius, GameObjInst * other);
	bool asteroidCollide(GameObjInst * me, GameObjInst * other);

protected:
	Asteroids m_game;
};
