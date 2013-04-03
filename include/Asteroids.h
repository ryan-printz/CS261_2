#pragma once

#include "GameObject.h"
#include <vector>

class Asteroids
{
public:
	Asteroids();

	GameObjInst * missileAcquireTarget(GameObjInst * pMissile);
	GameObjInst * astCreate(GameObjInst * pSrc);
	GameObjInst * gameObjInstCreate(unsigned type, float scale, AEVec2 * pPos, AEVec2 * pVel, float dir, bool forceCreate);

	void resolveCollision(GameObjInst * pSrc, GameObjInst * pDsg, AEVec2 * pNrm);
	void sparkCreate(u32 type, AEVec2 * pPos, u32 count, f32 angleMin, f32 angleMax, f32 srcSize = 1.0f, f32 velScale = 1.0f, AEVec2 * pVelInit = nullptr);
	void gameObjInstDestroy(GameObjInst * pInst);

	std::vector<GameObj> m_gameObjects;
	std::vector<GameObjInst> m_gameObjInsts;

	GameObjInst * m_localShip;
	float m_shipRotationSpeed;
	int	m_lives;
	int m_specials;
	int m_score;

	float m_gameTimer;
	float m_asteroidTimer;

	unsigned m_asteroids;
	unsigned m_maxAsteroids;
};
