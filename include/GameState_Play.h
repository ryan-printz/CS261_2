// ---------------------------------------------------------------------------
// Project Name		:	Asteroid Game
// File Name		:	GameState_Play.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	header file for the 'play' game state
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#pragma once

#include "IGameState.h"
#include "GameObject.h"

// ---------------------------------------------------------------------------

class GameState_Play : public IGameState
{
public:
	GameState_Play();
	virtual ~GameState_Play();
	virtual void load(void);
	virtual void init(void);
	virtual void update(void);
	virtual void draw(void);
	virtual void free(void);
	virtual void unload(void);

// private member functions
private:
	GameObjInst * missileAcquireTarget(GameObjInst * pMissile);
	GameObjInst * astCreate(GameObjInst * pSrc);
	GameObjInst * gameObjInstCreate(u32 type, f32 scale, AEVec2 * pPos, AEVec2* pVel, f32 dir, bool forceCreate);
	void resolveCollision(GameObjInst * pSrc, GameObjInst * pDsg, AEVec2 * pNrm);
	void sparkCreate(u32 type, AEVec2 * pPos, u32 count, f32 angleMin, f32 angleMax, f32 srcSize = 1.0f, f32 velScale = 1.0f, AEVec2 * pVelInit = nullptr);
	void gameObjInstDestroy(GameObjInst * pInst);
	void loadGameObjList();

// private member variables
private:
	// list of original object
	GameObj			sGameObjList[GAME_OBJ_NUM_MAX];
	int				sGameObjNum;

	// list of object instances
	GameObjInst		sGameObjInstList[GAME_OBJ_INST_NUM_MAX];
	int				sGameObjInstNum;

	// pointer ot the ship object
	GameObjInst *		spShip;

	// keep track when the last asteroid was created
	double				sAstCreationTime;

	// keep track the total number of asteroid active and the maximum allowed
	unsigned				sAstCtr;
	unsigned 				sAstNum;

	// current ship rotation speed
	float				sShipRotSpeed;

	// number of ship available (lives, 0 = game over)
	int				sShipCtr;

	// number of special attack
	int				sSpecialCtr;

	// the score = number of asteroid destroyed
	int				sScore;

	double			sGameStateChangeCtr;
};


