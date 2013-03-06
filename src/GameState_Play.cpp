// ---------------------------------------------------------------------------
// Project Name		:	Asteroid Game
// File Name		:	GameState_Play.cpp
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	implementation of the 'play' game state
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#include "main.h"

// ---------------------------------------------------------------------------
// Defines

#define GAME_OBJ_NUM_MAX			32
#define GAME_OBJ_INST_NUM_MAX		2048

#define AST_NUM_MIN					2		// minimum number of asteroid alive
#define AST_NUM_MAX					32		// maximum number of asteroid alive
#define AST_SIZE_MAX				20.0f	// maximum asterois size
#define AST_SIZE_MIN				5.0f	// minimum asterois size
#define AST_VEL_MAX					10.0f	// maximum asterois velocity
#define AST_VEL_MIN					5.0f	// minimum asterois velocity
#define AST_CREATE_DELAY			0.1f	// delay of creation between one asteroid to the next
#define AST_SPECIAL_RATIO			4		// number of asteroid for each 'special'
#define AST_SHIP_RATIO				100		// number of asteroid for each ship
#define AST_LIFE_MAX				10		// the life of the biggest asteroid (smaller one is scaled accordingly)
#define AST_VEL_DAMP				1E-18f	// dampening to use if the asteroid velocity is above the maximum
#define AST_TO_SHIP_ACC				0.001f	// how much acceleration to apply to steer the asteroid toward the ship

#define SHIP_INITIAL_NUM			3		// initial number of ship
#define SHIP_SPECIAL_NUM			20
#define SHIP_SIZE					3.0f	// ship size
#define SHIP_ACCEL_FORWARD			50.0f	// ship forward acceleration (in m/s^2)
#define SHIP_ACCEL_BACKWARD			-100.0f	// ship backward acceleration (in m/s^2)
#define SHIP_DAMP_FORWARD			0.55f 	// ship forward dampening
#define SHIP_DAMP_BACKWARD			0.05f 	// ship backward dampening
#define SHIP_ROT_SPEED				(1.0f * PI)	// ship rotation speed (degree/second)

#define BULLET_SPEED				100.0f	// bullet speed (m/s)

#define BOMB_COST					20		// cost to fire a bomb
#define BOMB_RADIUS					25.0f	// bomb radius
#define BOMB_LIFE					5.0f	// how many seconds the bomb will be alive

#define MISSILE_COST				1
#define MISSILE_ACCEL				1000.0f		// missile acceleration
#define MISSILE_TURN_SPEED			PI			// missile turning speed (radian/sec)
#define MISSILE_DAMP				1.5E-6f		// missile dampening
#define MISSILE_LIFE				10.0f		// how many seconds the missile will be alive

#define PTCL_SCALE_DAMP				0.05f	// particle scale dampening
#define PTCL_VEL_DAMP				0.05f	// particle velocity dampening

#define COLL_COEF_OF_RESTITUTION	1.0f	// collision coefficient of restitution

#define COLL_RESOLVE_SIMPLE			1

// ---------------------------------------------------------------------------
enum
{
	// list of game object types
	TYPE_SHIP = 0, 
	TYPE_BULLET, 
	TYPE_BOMB, 
	TYPE_MISSILE, 
	TYPE_ASTEROID, 
	TYPE_STAR, 

	TYPE_PTCL_WHITE, 
	TYPE_PTCL_YELLOW, 
	TYPE_PTCL_RED, 

	TYPE_NUM,

	PTCL_EXHAUST, 
	PTCL_EXPLOSION_S, 
	PTCL_EXPLOSION_M, 
	PTCL_EXPLOSION_L, 
};

// ---------------------------------------------------------------------------
// object flag definition

#define FLAG_ACTIVE		0x00000001
#define FLAG_LIFE_CTR_S	8
#define FLAG_LIFE_CTR_M	0x000000FF

// ---------------------------------------------------------------------------
// Struct/Class definitions

struct GameObj
{
	u32				type;		// object type
	AEGfxTriList*	pMesh;		// pbject
};

// ---------------------------------------------------------------------------

struct GameObjInst
{
	GameObj*		pObject;	// pointer to the 'original'
	u32				flag;		// bit flag or-ed together
	f32				life;		// object 'life'
	f32				scale;
	AEVec2			posCurr;	// object current position
	AEVec2			velCurr;	// object current velocity
	f32				dirCurr;	// object current direction

	AEMtx33			transform;	// object drawing matrix

	// pointer to custom data specific for each object type
	void*			pUserData;
};

// ---------------------------------------------------------------------------
// Static variables

// list of original object
static GameObj			sGameObjList[GAME_OBJ_NUM_MAX];
static u32				sGameObjNum;

// list of object instances
static GameObjInst		sGameObjInstList[GAME_OBJ_INST_NUM_MAX];
static u32				sGameObjInstNum;

// pointer ot the ship object
static GameObjInst*		spShip;

// keep track when the last asteroid was created
static f64				sAstCreationTime;

// keep track the total number of asteroid active and the maximum allowed
static u32				sAstCtr;
static u32				sAstNum;

// current ship rotation speed
static f32				sShipRotSpeed;

// number of ship available (lives, 0 = game over)
static s32				sShipCtr;

// number of special attack
static s32				sSpecialCtr;

// the score = number of asteroid destroyed
static u32				sScore;

static f64				sGameStateChangeCtr;

// ---------------------------------------------------------------------------

// function to 'load' object data
static void			loadGameObjList();

// function to create/destroy a game object object
static GameObjInst*	gameObjInstCreate (u32 type, f32 scale, AEVec2* pPos, AEVec2* pVel, f32 dir, bool forceCreate);
static void			gameObjInstDestroy(GameObjInst* pInst);

// function to create asteroid
static GameObjInst* astCreate(GameObjInst* pSrc);

// function to calculate the object's velocity after collison
static void			resolveCollision(GameObjInst* pSrc, GameObjInst* pDst, AEVec2* pNrm);

// function to create the particles
static void			sparkCreate(u32 type, AEVec2* pPos, u32 count, f32 angleMin, f32 angleMax, f32 srcSize = 0.0f, f32 velScale = 1.0f, AEVec2* pVelInit = 0);

// function for the missile to find a new target
static GameObjInst*	missileAcquireTarget(GameObjInst* pMissile);

// ---------------------------------------------------------------------------

void GameStatePlayLoad(void)
{
	// zero the game object list
	memset(sGameObjList, 0, sizeof(GameObj) * GAME_OBJ_NUM_MAX);
	sGameObjNum = 0;

	// zero the game object instance list
	memset(sGameObjInstList, 0, sizeof(GameObjInst) * GAME_OBJ_INST_NUM_MAX);
	sGameObjInstNum = 0;

	spShip = 0;

	// load/create the mesh data
	loadGameObjList();

	// initialize the initial number of asteroid
	sAstCtr = 0;
}

// ---------------------------------------------------------------------------

void GameStatePlayInit(void)
{
	// reset the number of current asteroid and the total allowed
	sAstCtr = 0;
	sAstNum = AST_NUM_MIN;

	// create the main ship
	spShip = gameObjInstCreate(TYPE_SHIP, SHIP_SIZE, 0, 0, 0.0f, true);
	AE_ASSERT(spShip);

	// get the time the asteroid is created
	sAstCreationTime = AEGetTime();
	
	// generate the initial asteroid
	for (u32 i = 0; i < sAstNum; i++)
		astCreate(0);

	// reset the score and the number of ship
	sScore      = 0;
	sShipCtr    = SHIP_INITIAL_NUM;
	sSpecialCtr = SHIP_SPECIAL_NUM;

	// reset the delay to switch to the result state after game over
	sGameStateChangeCtr = 2.0f;
}

// ---------------------------------------------------------------------------

void GameStatePlayUpdate(void)
{
	// =================
	// update the input
	// =================

	if (spShip == 0)
	{
		sGameStateChangeCtr -= (f32)(gAEFrameTime);

		if (sGameStateChangeCtr < 0.0)
			gGameStateNext = GS_RESULT;
	}
	else
	{
		if (AEInputCheckCurr(DIK_UP))
		{
#if 0
			AEVec2 acc, u, dir;

			// calculate the current direction vector
			AEVec2Set	(&dir, AECos(spShip->dirCurr), AESin(spShip->dirCurr));

			// calculate the dampening vector
			AEVec2Scale(&u, &spShip->velCurr, -AEVec2Length(&spShip->velCurr) * 0.01f);//pow(SHIP_DAMP_FORWARD, (f32)(gAEFrameTime)));

			// calculate the acceleration vector and add the dampening vector to it
			//AEVec2Scale	(&acc, &dir, 0.5f * SHIP_ACCEL_FORWARD * (f32)(gAEFrameTime) * (f32)(gAEFrameTime));
			AEVec2Scale	(&acc, &dir, SHIP_ACCEL_FORWARD);
			AEVec2Add	(&acc, &acc, &u);

			// add the velocity to the position
			//AEVec2Scale	(&u,               &spShip->velCurr, (f32)(gAEFrameTime));
			//AEVec2Add	(&spShip->posCurr, &spShip->posCurr, &u);
			// add the acceleration to the position
			AEVec2Scale	(&u,               &acc,             0.5f * (f32)(gAEFrameTime) * (f32)(gAEFrameTime));
			AEVec2Add	(&spShip->posCurr, &spShip->posCurr, &u);

			// add the acceleration to the velocity
			AEVec2Scale	(&u,               &acc, (f32)(gAEFrameTime));
			AEVec2Add	(&spShip->velCurr, &acc, &spShip->velCurr);

			AEVec2Scale	(&u, &dir, -spShip->scale);
			AEVec2Add	(&u, &u,   &spShip->posCurr);

			sparkCreate(PTCL_EXHAUST, &u, 2, spShip->dirCurr + 0.8f * PI, spShip->dirCurr + 1.2f * PI);
#else
			AEVec2 pos, dir;

			AEVec2Set	(&dir, AECos(spShip->dirCurr), AESin(spShip->dirCurr));
			pos = dir;
			AEVec2Scale	(&dir, &dir, SHIP_ACCEL_FORWARD * (f32)(gAEFrameTime));
			AEVec2Add	(&spShip->velCurr, &spShip->velCurr, &dir);
			AEVec2Scale	(&spShip->velCurr, &spShip->velCurr, pow(SHIP_DAMP_FORWARD, (f32)(gAEFrameTime)));

			AEVec2Scale	(&pos, &pos, -spShip->scale);
			AEVec2Add	(&pos, &pos, &spShip->posCurr);

			sparkCreate(PTCL_EXHAUST, &pos, 2, spShip->dirCurr + 0.8f * PI, spShip->dirCurr + 1.2f * PI);
#endif
		}
		if (AEInputCheckCurr(DIK_DOWN))
		{
			AEVec2 dir;
			
			AEVec2Set	(&dir, AECos(spShip->dirCurr), AESin(spShip->dirCurr));
			AEVec2Scale	(&dir, &dir, SHIP_ACCEL_BACKWARD * (f32)(gAEFrameTime));
			AEVec2Add	(&spShip->velCurr, &spShip->velCurr, &dir);
			AEVec2Scale	(&spShip->velCurr, &spShip->velCurr, pow(SHIP_DAMP_BACKWARD, (f32)(gAEFrameTime)));
		}
		if (AEInputCheckCurr(DIK_LEFT))
		{
			sShipRotSpeed   += (SHIP_ROT_SPEED - sShipRotSpeed) * 0.1f;
			spShip->dirCurr += sShipRotSpeed * (f32)(gAEFrameTime);
			spShip->dirCurr =  AEWrap(spShip->dirCurr, -PI, PI);
		}
		else if (AEInputCheckCurr(DIK_RIGHT))
		{
			sShipRotSpeed   += (SHIP_ROT_SPEED - sShipRotSpeed) * 0.1f;
			spShip->dirCurr -= sShipRotSpeed * (f32)(gAEFrameTime);
			spShip->dirCurr =  AEWrap(spShip->dirCurr, -PI, PI);
		}
		else
		{
			sShipRotSpeed = 0.0f;
		}
		if (AEInputCheckTriggered(DIK_SPACE))
		{
			AEVec2 vel;

			AEVec2Set	(&vel, AECos(spShip->dirCurr), AESin(spShip->dirCurr));
			AEVec2Scale	(&vel, &vel, BULLET_SPEED);
			
			gameObjInstCreate(TYPE_BULLET, 1.0f, &spShip->posCurr, &vel, spShip->dirCurr, true);
		}
		if (AEInputCheckTriggered(DIK_Z) && (sSpecialCtr >= BOMB_COST))
		{
			u32 i;

			// make sure there is no bomb is active currently
			for (i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
				if ((sGameObjInstList[i].flag & FLAG_ACTIVE) && (sGameObjInstList[i].pObject->type == TYPE_BOMB))
					break;

			// if no bomb is active currently, create one
			if (i == GAME_OBJ_INST_NUM_MAX)
			{
				sSpecialCtr -= BOMB_COST;
				gameObjInstCreate(TYPE_BOMB, 1.0f, &spShip->posCurr, 0, 0, true);
			}
		}
		if (AEInputCheckTriggered(DIK_X) && (sSpecialCtr >= MISSILE_COST))
		{
			sSpecialCtr -= MISSILE_COST;

			f32			 dir = spShip->dirCurr;
			AEVec2       vel = spShip->velCurr;
			AEVec2		 pos;

			AEVec2Set	(&pos, AECos(spShip->dirCurr), AESin(spShip->dirCurr));
			AEVec2Scale	(&pos, &pos, spShip->scale * 0.5f);
			AEVec2Add	(&pos, &pos, &spShip->posCurr);
			
			gameObjInstCreate(TYPE_MISSILE, 1.0f, &pos, &vel, dir, true);
		}
	}

	// ==================================
	// create new asteroids if necessary
	// ==================================

	if ((sAstCtr < sAstNum) && ((AEGetTime() - sAstCreationTime) > AST_CREATE_DELAY))
	{
		// keep track the last time an asteroid is created
		sAstCreationTime = AEGetTime();

		// create an asteroid
		astCreate(0);
	}

	// ===============
	// update physics
	// ===============

	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		
		// update the position
		AEVec2ScaleAdd(&pInst->posCurr, &pInst->velCurr, &pInst->posCurr, (f32)(gAEFrameTime));
	}

	// ===============
	// update objects
	// ===============

	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		
		// check if the object is a ship
		if (pInst->pObject->type == TYPE_SHIP)
		{
			// warp the ship from one end of the screen to the other
			pInst->posCurr.x = AEWrap(pInst->posCurr.x, gAEWinMinX - SHIP_SIZE, gAEWinMaxX + SHIP_SIZE);
			pInst->posCurr.y = AEWrap(pInst->posCurr.y, gAEWinMinY - SHIP_SIZE, gAEWinMaxY + SHIP_SIZE);
		}
		// check if the object is an asteroid
		else if (pInst->pObject->type == TYPE_ASTEROID)
		{
			AEVec2 u;
			f32    uLen;

			// warp the asteroid from one end of the screen to the other
			pInst->posCurr.x = AEWrap(pInst->posCurr.x, gAEWinMinX - AST_SIZE_MAX, gAEWinMaxX + AST_SIZE_MAX);
			pInst->posCurr.y = AEWrap(pInst->posCurr.y, gAEWinMinY - AST_SIZE_MAX, gAEWinMaxY + AST_SIZE_MAX);

			// pull the asteroid toward the ship a little bit
			if (spShip)
			{
				// apply acceleration propotional to the distance from the asteroid to the ship
				AEVec2Sub	(&u, &spShip->posCurr, &pInst->posCurr);
				AEVec2Scale	(&u, &u, AST_TO_SHIP_ACC * (f32)(gAEFrameTime));
				AEVec2Add	(&pInst->velCurr, &pInst->velCurr, &u);
			}

			// if the asterid velocity is more than its maximum velocity, reduce its speed
			if ((uLen = AEVec2Length(&pInst->velCurr)) > (AST_VEL_MAX * 2.0f))
			{
				AEVec2Scale	(&u, &pInst->velCurr, (1.0f / uLen) * (AST_VEL_MAX * 2.0f - uLen) * pow(AST_VEL_DAMP, (f32)(gAEFrameTime)));
				AEVec2Add	(&pInst->velCurr, &pInst->velCurr, &u);
			}
		}
		// check if the object is a bullet
		else if (pInst->pObject->type == TYPE_BULLET)
		{
			// kill the bullet if it gets out of the screen
			if (!AEInRange(pInst->posCurr.x, gAEWinMinX - AST_SIZE_MAX, gAEWinMaxX + AST_SIZE_MAX) || 
				!AEInRange(pInst->posCurr.y, gAEWinMinY - AST_SIZE_MAX, gAEWinMaxY + AST_SIZE_MAX))
				gameObjInstDestroy(pInst);
		}
		// check if the object is a bomb
		else if (pInst->pObject->type == TYPE_BOMB)
		{
			// adjust the life counter
			pInst->life -= (f32)(gAEFrameTime) / BOMB_LIFE;
			
			if (pInst->life < 0.0f)
			{
				gameObjInstDestroy(pInst);
			}
			else
			{
				f32    radius = 1.0f - pInst->life;
				AEVec2 u;

				pInst->dirCurr += 2.0f * PI * (f32)(gAEFrameTime);

				radius =   1.0f - radius;
				radius *=  radius;
				radius *=  radius;
				radius *=  radius;
				radius *=  radius;
				radius =   (1.0f - radius) * BOMB_RADIUS;

				// generate the particle ring
				for (u32 j = 0; j < 10; j++)
				{
					//f32 dir = AERandFloat() * 2.0f * PI;
					f32 dir = (j / 9.0f) * 2.0f * PI + pInst->life * 1.5f * 2.0f * PI;

					u.x = AECos(dir) * radius + pInst->posCurr.x;
					u.y = AESin(dir) * radius + pInst->posCurr.y;

					//sparkCreate(PTCL_EXHAUST, &u, 1, dir + 0.8f * PI, dir + 0.9f * PI);
					sparkCreate(PTCL_EXHAUST, &u, 1, dir + 0.40f * PI, dir + 0.60f * PI);
				}
			}
		}
		// check if the object is a missile
		else if (pInst->pObject->type == TYPE_MISSILE)
		{
			// adjust the life counter
			pInst->life -= (f32)(gAEFrameTime) / MISSILE_LIFE;
			
			if (pInst->life < 0.0f)
			{
				gameObjInstDestroy(pInst);
			}
			else
			{
				AEVec2 dir;

				if (pInst->pUserData == 0)
				{
					pInst->pUserData = missileAcquireTarget(pInst);
				}
				else
				{
					GameObjInst* pTarget = (GameObjInst*)(pInst->pUserData);

					// if the target is no longer valid, reacquire
					if (((pTarget->flag & FLAG_ACTIVE) == 0) || (pTarget->pObject->type != TYPE_ASTEROID))
						pInst->pUserData = missileAcquireTarget(pInst);
				}

				if (pInst->pUserData)
				{
					GameObjInst* pTarget = (GameObjInst*)(pInst->pUserData);
					AEVec2 u;
					f32    uLen;

					// get the vector from the missile to the target and its length
					AEVec2Sub(&u, &pTarget->posCurr, &pInst->posCurr);
					uLen = AEVec2Length(&u);

					// if the missile is 'close' to target, do nothing
					if (uLen > 0.1f)
					{
						// normalize the vector from the missile to the target
						AEVec2Scale(&u, &u, 1.0f / uLen);

						// calculate the missile direction vector
						AEVec2Set(&dir, AECos(pInst->dirCurr), AESin(pInst->dirCurr));

						// calculate the cos and sin of the angle between the target 
						// vector and the missile direction vector
						f32 cosAngle = AEVec2DotProduct(&dir, &u), 
							sinAngle = AEVec2CrossProductMag(&dir, &u), 
							rotAngle;

						// calculate how much to rotate the missile
						if (cosAngle < AECos(MISSILE_TURN_SPEED * (f32)(gAEFrameTime)))
							rotAngle = MISSILE_TURN_SPEED * (f32)(gAEFrameTime);
						else
							rotAngle = AEACos(AEClamp(cosAngle, -1.0f, 1.0f));

						// rotate to the left if sine of the angle is positive and vice versa
						pInst->dirCurr += (sinAngle > 0.0f) ? rotAngle : -rotAngle;
					}
				}

				// adjust the missile velocity
				AEVec2Set  (&dir, AECos(pInst->dirCurr), AESin(pInst->dirCurr));
				AEVec2Scale(&dir, &dir, MISSILE_ACCEL * (f32)(gAEFrameTime));
				AEVec2Add  (&pInst->velCurr, &pInst->velCurr, &dir);
				AEVec2Scale(&pInst->velCurr, &pInst->velCurr, pow(MISSILE_DAMP, (f32)(gAEFrameTime)));

				sparkCreate(PTCL_EXHAUST, &pInst->posCurr, 1, pInst->dirCurr + 0.8f * PI, pInst->dirCurr + 1.2f * PI);
			}
		}
		// check if the object is a particle
		else if ((TYPE_PTCL_WHITE <= pInst->pObject->type) && (pInst->pObject->type <= TYPE_PTCL_RED))
		{
			pInst->scale   *= pow(PTCL_SCALE_DAMP, (f32)(gAEFrameTime));
			pInst->dirCurr += 0.1f;
			AEVec2Scale(&pInst->velCurr, &pInst->velCurr, pow(PTCL_VEL_DAMP, (f32)(gAEFrameTime)));

			if (pInst->scale < PTCL_SCALE_DAMP)
				gameObjInstDestroy(pInst);
		}
	}

	// ====================
	// check for collision
	// ====================

	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pSrc = sGameObjInstList + i;

		// skip non-active object
		if ((pSrc->flag & FLAG_ACTIVE) == 0)
			continue;

		if ((pSrc->pObject->type == TYPE_BULLET) || (pSrc->pObject->type == TYPE_MISSILE))
		{
			for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
			{
				GameObjInst* pDst = sGameObjInstList + j;

				// skip no-active and non-asteroid object
				if (((pDst->flag & FLAG_ACTIVE) == 0) || (pDst->pObject->type != TYPE_ASTEROID))
					continue;

				if (AETestPointToRect(&pSrc->posCurr, &pDst->posCurr, pDst->scale, pDst->scale) == false)
					continue;
				
				if (pDst->scale < AST_SIZE_MIN)
				{
					sparkCreate(PTCL_EXPLOSION_M, &pDst->posCurr, (u32)(pDst->scale * 10), pSrc->dirCurr - 0.05f * PI, pSrc->dirCurr + 0.05f * PI, pDst->scale);
					sScore++;

					if ((sScore % AST_SPECIAL_RATIO) == 0)
						sSpecialCtr++;
					if ((sScore % AST_SHIP_RATIO) == 0)
						sShipCtr++;
					if (sScore == sAstNum * 5)
						sAstNum = (sAstNum < AST_NUM_MAX) ? (sAstNum * 2) : sAstNum;

					// destroy the asteroid
					gameObjInstDestroy(pDst);
				}
				else
				{
					sparkCreate(PTCL_EXPLOSION_S, &pSrc->posCurr, 10, pSrc->dirCurr + 0.9f * PI, pSrc->dirCurr + 1.1f * PI);

					// impart some of the bullet/missile velocity to the asteroid
					AEVec2Scale(&pSrc->velCurr, &pSrc->velCurr, 0.01f * (1.0f - pDst->scale / AST_SIZE_MAX));
					AEVec2Add  (&pDst->velCurr, &pDst->velCurr, &pSrc->velCurr);

					// split the asteroid to 4
					if ((pSrc->pObject->type == TYPE_MISSILE) || ((pDst->life -= 1.0f) < 0.0f))
						astCreate(pDst);
				}

				// destroy the bullet
				gameObjInstDestroy(pSrc);

				break;
			}
		}
		else if (TYPE_BOMB == pSrc->pObject->type)
		{
			f32 radius = 1.0f - pSrc->life;

			pSrc->dirCurr += 2.0f * PI * (f32)(gAEFrameTime);

			radius =   1.0f - radius;
			radius *=  radius;
			radius *=  radius;
			radius *=  radius;
			radius *=  radius;
			radius *=  radius;
			radius =   (1.0f - radius) * BOMB_RADIUS;

			// check collision
			for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
			{
				GameObjInst* pDst = sGameObjInstList + j;

				if (((pDst->flag & FLAG_ACTIVE) == 0) || (pDst->pObject->type != TYPE_ASTEROID))
					continue;

				if (AECalcDistPointToRect(&pSrc->posCurr, &pDst->posCurr, pDst->scale, pDst->scale) > radius)
					continue;

				if (pDst->scale < AST_SIZE_MIN)
				{
					f32 dir = atan2f(pDst->posCurr.y - pSrc->posCurr.y, pDst->posCurr.x - pSrc->posCurr.x);

					gameObjInstDestroy(pDst);
					sparkCreate(PTCL_EXPLOSION_M, &pDst->posCurr, 20, dir + 0.4f * PI, dir + 0.45f * PI);
					sScore++;

					if ((sScore % AST_SPECIAL_RATIO) == 0)
						sSpecialCtr++;
					if ((sScore % AST_SHIP_RATIO) == 0)
						sShipCtr++;
					if (sScore == sAstNum * 5)
						sAstNum = (sAstNum < AST_NUM_MAX) ? (sAstNum * 2) : sAstNum;
				}
				else
				{
					// split the asteroid to 4
					astCreate(pDst);
				}
			}
		}
		else if (pSrc->pObject->type == TYPE_ASTEROID)
		{
			for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
			{
				GameObjInst* pDst = sGameObjInstList + j;
				f32          d;
				AEVec2       nrm, u;

				// skip no-active and non-asteroid object
				if ((pSrc == pDst) || ((pDst->flag & FLAG_ACTIVE) == 0)  || (pDst->pObject->type != TYPE_ASTEROID))
					continue;

				// check if the object rectangle overlap
				d = AECalcDistRectToRect(
					&pSrc->posCurr, pSrc->scale, pSrc->scale, 
					&pDst->posCurr, pDst->scale, pDst->scale, 
					&nrm);
				
				if (d >= 0.0f)
					continue;
				
				// adjust object position so that they do not overlap
				AEVec2Scale	(&u, &nrm, d * 0.25f);
				AEVec2Sub	(&pSrc->posCurr, &pSrc->posCurr, &u);
				AEVec2Add	(&pDst->posCurr, &pDst->posCurr, &u);

				// calculate new object velocities
				resolveCollision(pSrc, pDst, &nrm);
			}
		}
		else if (pSrc->pObject->type == TYPE_SHIP)
		{
			for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
			{
				GameObjInst* pDst = sGameObjInstList + j;

				// skip no-active and non-asteroid object
				if ((pSrc == pDst) || ((pDst->flag & FLAG_ACTIVE) == 0) || (pDst->pObject->type != TYPE_ASTEROID))
					continue;

				// check if the object rectangle overlap
				if (AETestRectToRect(
					&pSrc->posCurr, pSrc->scale, pSrc->scale, 
					&pDst->posCurr, pDst->scale, pDst->scale) == false)
					continue;

				// create the big explosion
				sparkCreate(PTCL_EXPLOSION_L, &pSrc->posCurr, 100, 0.0f, 2.0f * PI);
				
				// reset the ship position and direction
				AEVec2Zero(&spShip->posCurr);
				AEVec2Zero(&spShip->velCurr);
				spShip->dirCurr = 0.0f;

				sSpecialCtr = SHIP_SPECIAL_NUM;

				// destroy all asteroid near the ship so that you do not die as soon as the ship reappear
				for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
				{
					GameObjInst* pInst = sGameObjInstList + j;
					AEVec2		 u;

					// skip no-active and non-asteroid object
					if (((pInst->flag & FLAG_ACTIVE) == 0) || (pInst->pObject->type != TYPE_ASTEROID))
						continue;

					AEVec2Sub(&u, &pInst->posCurr, &spShip->posCurr);

					if (AEVec2Length(&u) < (spShip->scale * 10.0f))
					{
						sparkCreate		  (PTCL_EXPLOSION_M, &pInst->posCurr, 10, -PI, PI);
						gameObjInstDestroy(pInst);
					}
				}

				// reduce the ship counter
				sShipCtr--;
				
				// if counter is less than 0, game over
				if (sShipCtr < 0)
				{
					sGameStateChangeCtr = 2.0;
					gameObjInstDestroy(spShip);
					spShip = 0;
				}

				break;
			}
		}
	}

	// =====================================
	// calculate the matrix for all objects
	// =====================================

	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;
		AEMtx33		 m;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		
		AEMtx33Scale		(&pInst->transform, pInst->scale,     pInst->scale);
		AEMtx33Rot			(&m,                pInst->dirCurr);
		AEMtx33Concat		(&pInst->transform, &m,               &pInst->transform);
		AEMtx33Trans		(&m,                pInst->posCurr.x, pInst->posCurr.y);
		AEMtx33Concat		(&pInst->transform, &m,               &pInst->transform);
	}
}

// ---------------------------------------------------------------------------

void GameStatePlayDraw(void)
{
	s8 strBuffer[1024];

	// draw all object in the list
	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		
		AEGfxSetTransform	(&sGameObjInstList[i].transform);
		AEGfxTriDraw		(sGameObjInstList[i].pObject->pMesh);
	}

	sprintf(strBuffer, "Score: %d", sScore);
	AEGfxPrint(10, 10, -1, strBuffer);

	sprintf(strBuffer, "Level: %d", AELogBase2(sAstNum));
	AEGfxPrint(10, 30, -1, strBuffer);

	sprintf(strBuffer, "Ship Left: %d", sShipCtr >= 0 ? sShipCtr : 0);
	AEGfxPrint(600, 10, -1, strBuffer);

	sprintf(strBuffer, "Special:   %d", sSpecialCtr);
	AEGfxPrint(600, 30, -1, strBuffer);

	// display the game over message
	if (sShipCtr < 0)
		AEGfxPrint(280, 260, 0xFFFFFFFF, "       GAME OVER       ");
}

// ---------------------------------------------------------------------------

void GameStatePlayFree(void)
{
	// kill all object in the list
	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
		gameObjInstDestroy(sGameObjInstList + i);

	// reset asteroid count
	sAstCtr = 0;
}

// ---------------------------------------------------------------------------

void GameStatePlayUnload(void)
{
	// free all mesh
	for (u32 i = 0; i < sGameObjNum; i++)
		AEGfxTriFree(sGameObjList[i].pMesh);
}

// ---------------------------------------------------------------------------
// Static function implementations

static void loadGameObjList()
{
	GameObj* pObj;

	// ================
	// create the ship
	// ================

	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_SHIP;

	AEGfxTriStart();
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFF0000, 
		 0.5f,  0.0f, 0xFFFFFFFF, 
		-0.5f,  0.5f, 0xFFFF0000);

	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "fail to create object!!");

	// ==================
	// create the bullet
	// ==================

	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_BULLET;

	AEGfxTriStart();
	AEGfxTriAdd(
		-1.0f,  0.2f, 0x00FFFF00, 
		-1.0f, -0.2f, 0x00FFFF00, 
		 1.0f, -0.2f, 0xFFFFFF00);
	AEGfxTriAdd(
		-1.0f,  0.2f, 0x00FFFF00, 
		 1.0f, -0.2f, 0xFFFFFF00, 
		 1.0f,  0.2f, 0xFFFFFF00);

	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "fail to create object!!");

	// ==================
	// create the bomb
	// ==================

	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_BOMB;

	AEGfxTriStart();
	AEGfxTriAdd(
		-1.0f,  1.0f, 0xFFFF8000, 
		-1.0f, -1.0f, 0xFFFF8000, 
		 1.0f, -1.0f, 0xFFFF8000);
	AEGfxTriAdd(
		-1.0f,  1.0f, 0xFFFF8000, 
		 1.0f, -1.0f, 0xFFFF8000, 
		 1.0f,  1.0f, 0xFFFF8000);

	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "fail to create object!!");

	// ==================
	// create the missile
	// ==================

	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_MISSILE;

	AEGfxTriStart();
	AEGfxTriAdd(
		-1.0f, -0.5f, 0xFFFF0000, 
		 1.0f,  0.0f, 0xFFFFFF00, 
		-1.0f,  0.5f, 0xFFFF0000);

	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "fail to create object!!");

	// ====================
	// create the asteroid
	// ====================

	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_ASTEROID;

	AEGfxTriStart();
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFF808080, 
		 0.5f,  0.5f, 0xFF808080, 
		-0.5f,  0.5f, 0xFF808080);
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFF808080, 
		 0.5f, -0.5f, 0xFF808080, 
		 0.5f,  0.5f, 0xFF808080);

	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "fail to create object!!");

	// ====================
	// create the star
	// ====================

	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_STAR;

	AEGfxTriStart();
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFF8080FF, 
		 0.5f,  0.5f, 0xFF8080FF, 
		-0.5f,  0.5f, 0xFF8080FF);
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFF8080FF, 
		 0.5f, -0.5f, 0xFF8080FF, 
		 0.5f,  0.5f, 0xFF8080FF);

	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "fail to create object!!");

	// ================
	// create the ptcl
	// ================

	for (u32 i = 0; i < 3; i++)
	{
		u32 color = (i == 0) ? (0xFFFFFFFF) : (
					(i == 1) ? (0xFFFFFF00) : (0xFFFF0000));

		pObj		= sGameObjList + sGameObjNum++;
		pObj->type	= TYPE_PTCL_WHITE + i;

		AEGfxTriStart();
		AEGfxTriAdd(
			-1.0f * (3 - i), -0.5f * (3 - i), color, 
			 1.0f * (3 - i),  0.5f * (3 - i), color, 
			-1.0f * (3 - i),  0.5f * (3 - i), color);
		AEGfxTriAdd(
			-1.0f * (3 - i), -0.5f * (3 - i), color, 
			 1.0f * (3 - i), -0.5f * (3 - i), color, 
			 1.0f * (3 - i),  0.5f * (3 - i), color);

		pObj->pMesh = AEGfxTriEnd();
		AE_ASSERT_MESG(pObj->pMesh, "fail to create object!!");
	}
}

// ---------------------------------------------------------------------------

GameObjInst* gameObjInstCreate(u32 type, f32 scale, AEVec2* pPos, AEVec2* pVel, f32 dir, bool forceCreate)
{
	AEVec2 zero = { 0.0f, 0.0f };

	AE_ASSERT_PARM(type < sGameObjNum);
	
	// loop through the object instance list to find a non-used object instance
	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// check if current instance is not used
		if (pInst->flag == 0)
		{
			// it is not used => use it to create the new instance
			pInst->pObject	 = sGameObjList + type;
			pInst->flag		 = FLAG_ACTIVE;
			pInst->life		 = 1.0f;
			pInst->scale	 = scale;
			pInst->posCurr	 = pPos ? *pPos : zero;
			pInst->velCurr	 = pVel ? *pVel : zero;
			pInst->dirCurr	 = dir;
			pInst->pUserData = 0;

			// keep track the number of asteroid
			if (pInst->pObject->type == TYPE_ASTEROID)
				sAstCtr++;
			
			// return the newly created instance
			return pInst;
		}
	}

	if (forceCreate)
	{
		f32          scaleMin = FLT_MAX;
		GameObjInst* pDst     = 0;

		// loop through the object instance list to find the smallest particle
		for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
		{
			GameObjInst* pInst = sGameObjInstList + i;

			// check if current instance is a red particle
			if ((TYPE_PTCL_RED <= pInst->pObject->type) && (pInst->pObject->type <= TYPE_PTCL_WHITE) && (pInst->scale < scaleMin))
			{
				scaleMin = pInst->scale;
				pDst     = pInst;
			}
		}

		if (pDst)
		{
			pDst->pObject	 = sGameObjList + type;
			pDst->flag		 = FLAG_ACTIVE;
			pDst->life		 = 1.0f;
			pDst->scale	 = scale;
			pDst->posCurr	 = pPos ? *pPos : zero;
			pDst->velCurr	 = pVel ? *pVel : zero;
			pDst->dirCurr	 = dir;
			pDst->pUserData = 0;

			// keep track the number of asteroid
			if (pDst->pObject->type == TYPE_ASTEROID)
				sAstCtr++;
			
			// return the newly created instance
			return pDst;
		}
	}

	// cannot find empty slot => return 0
	return 0;
}

// ---------------------------------------------------------------------------

void gameObjInstDestroy(GameObjInst* pInst)
{
	// if instance is destroyed before, just return
	if (pInst->flag == 0)
		return;

	// zero out the flag
	pInst->flag = 0;
	
	// keep track the number of asteroid
	if (pInst->pObject->type == TYPE_ASTEROID)
		sAstCtr--;
}

// ---------------------------------------------------------------------------

GameObjInst* astCreate(GameObjInst* pSrc)
{
	GameObjInst* pInst;
	AEVec2		 pos, vel;
	f32			 t, angle, size;

	if (pSrc)
	{
		f32		posOffset = pSrc->scale * 0.25f;
		f32		velOffset = (AST_SIZE_MAX - pSrc->scale + 1.0f) * 0.25f;
		f32		scaleNew  = pSrc->scale * 0.5f;

		sparkCreate(PTCL_EXPLOSION_L, &pSrc->posCurr, 5, 0.0f * PI - 0.01f * PI, 0.0f * PI + 0.01f * PI, 0.0f, pSrc->scale / AST_SIZE_MAX, &pSrc->velCurr);
		sparkCreate(PTCL_EXPLOSION_L, &pSrc->posCurr, 5, 0.5f * PI - 0.01f * PI, 0.5f * PI + 0.01f * PI, 0.0f, pSrc->scale / AST_SIZE_MAX, &pSrc->velCurr);
		sparkCreate(PTCL_EXPLOSION_L, &pSrc->posCurr, 5, 1.0f * PI - 0.01f * PI, 1.0f * PI + 0.01f * PI, 0.0f, pSrc->scale / AST_SIZE_MAX, &pSrc->velCurr);
		sparkCreate(PTCL_EXPLOSION_L, &pSrc->posCurr, 5, 1.5f * PI - 0.01f * PI, 1.5f * PI + 0.01f * PI, 0.0f, pSrc->scale / AST_SIZE_MAX, &pSrc->velCurr);

		pInst = astCreate(0);
		pInst->scale = scaleNew;
		AEVec2Set(&pInst->posCurr, pSrc->posCurr.x - posOffset, pSrc->posCurr.y - posOffset);
		AEVec2Set(&pInst->velCurr, pSrc->velCurr.x - velOffset, pSrc->velCurr.y - velOffset);

		pInst = astCreate(0);
		pInst->scale = scaleNew;
		AEVec2Set(&pInst->posCurr, pSrc->posCurr.x + posOffset, pSrc->posCurr.y - posOffset);
		AEVec2Set(&pInst->velCurr, pSrc->velCurr.x + velOffset, pSrc->velCurr.y - velOffset);

		pInst = astCreate(0);
		pInst->scale = scaleNew;
		AEVec2Set(&pInst->posCurr, pSrc->posCurr.x - posOffset, pSrc->posCurr.y + posOffset);
		AEVec2Set(&pInst->velCurr, pSrc->velCurr.x - velOffset, pSrc->velCurr.y + velOffset);

		pSrc->scale = scaleNew;
		AEVec2Set(&pSrc->posCurr, pSrc->posCurr.x + posOffset, pSrc->posCurr.y + posOffset);
		AEVec2Set(&pSrc->velCurr, pSrc->velCurr.x + velOffset, pSrc->velCurr.y + velOffset);
		
		return pSrc;
	}

	// pick a random angle and velocity magnitude
	angle = AERandFloat() * 2.0f * PI;
	size  = AERandFloat() * (AST_SIZE_MAX - AST_SIZE_MIN) + AST_SIZE_MIN;

	// pick a random position along the top or left edge
	if ((t = AERandFloat()) < 0.5f)
		AEVec2Set(&pos, gAEWinMinX + (t * 2.0f) * (gAEWinMaxX - gAEWinMinX), gAEWinMinY - size * 0.5f);
	else
		AEVec2Set(&pos, gAEWinMinX - size * 0.5f, gAEWinMinY + ((t - 0.5f) * 2.0f) * (gAEWinMaxY - gAEWinMinY));

	// calculate the velocity vector
	AEVec2Set	(&vel, AECos(angle), AESin(angle));
	AEVec2Scale	(&vel, &vel, AERandFloat() * (AST_VEL_MAX - AST_VEL_MIN) + AST_VEL_MIN);

	// create the object instance
	pInst = gameObjInstCreate(TYPE_ASTEROID, size, &pos, &vel, 0.0f, true);
	AE_ASSERT(pInst);

	// set the life based on the size
	pInst->life = size / AST_SIZE_MAX * AST_LIFE_MAX;

	return pInst;
}

// ---------------------------------------------------------------------------

void resolveCollision(GameObjInst* pSrc, GameObjInst* pDst, AEVec2* pNrm)
{
#if COLL_RESOLVE_SIMPLE

	f32 ma = pSrc->scale * pSrc->scale, 
		mb = pDst->scale * pDst->scale, 
		e  = COLL_COEF_OF_RESTITUTION;

	if (pNrm->y == 0)// EPSILON)
	{
		// calculate the relative velocity of the 1st object againts the 2nd object along the x-axis
		f32 velRel = pSrc->velCurr.x - pDst->velCurr.x;

		// if the object is separating, do nothing
		if((velRel * pNrm->x) >= 0.0f)
			return;

		pSrc->velCurr.x = (ma * pSrc->velCurr.x + mb * (pDst->velCurr.x - e * velRel)) / (ma + mb);
		pDst->velCurr.x = pSrc->velCurr.x + e * velRel;
	}
	else
	{
		// calculate the relative velocity of the 1st object againts the 2nd object along the y-axis
		f32 velRel = pSrc->velCurr.y - pDst->velCurr.y;

		// if the object is separating, do nothing
		if((velRel * pNrm->y) >= 0.0f)
			return;

		pSrc->velCurr.y = (ma * pSrc->velCurr.y + mb * (pDst->velCurr.y - e * velRel)) / (ma + mb);
		pDst->velCurr.y = pSrc->velCurr.y + e * velRel;
	}

#else

	f32		ma = pSrc->scale * pSrc->scale, 
			mb = pDst->scale * pDst->scale, 
			e  = COLL_COEF_OF_RESTITUTION;
	AEVec2	u;
	AEVec2	velSrc, velDst;
	f32		velRel;

	// calculate the relative velocity of the 1st object againts the 2nd object
	AEVec2Sub(&u, &pSrc->velCurr, &pDst->velCurr);

	// if the object is separating, do nothing
	if(AEVec2DotProduct(&u, pNrm) > 0.0f)
		return;

	// calculate the side vector (pNrm rotated by 90 degree)
	AEVec2Set(&u, -pNrm->y, pNrm->x);
	
	// tranform the object velocities to the plane space
	velSrc.x	=	AEVec2DotProduct(&pSrc->velCurr, &u);
	velSrc.y	=	AEVec2DotProduct(&pSrc->velCurr, pNrm);
	velDst.x	=	AEVec2DotProduct(&pDst->velCurr, &u);
	velDst.y	=	AEVec2DotProduct(&pDst->velCurr, pNrm);

	// calculate the relative velocity along the y axis
	velRel		=	velSrc.y - velDst.y;

	// resolve collision along the Y axis
	velSrc.y = (ma * velSrc.y + mb * (velDst.y - e * velRel)) / (ma + mb);
	velDst.y = velSrc.y + e * velRel;

	// tranform back the velocity from the normal space to the world space
	AEVec2Scale		(&pSrc->velCurr, pNrm, velSrc.y);
	AEVec2ScaleAdd	(&pSrc->velCurr, &u, &pSrc->velCurr, velSrc.x);
	AEVec2Scale		(&pDst->velCurr, pNrm, velDst.y);
	AEVec2ScaleAdd	(&pDst->velCurr, &u, &pDst->velCurr, velDst.x);

#endif // COLL_RESOLVE_SIMPLE
}

// ---------------------------------------------------------------------------

void sparkCreate(u32 type, AEVec2* pPos, u32 count, f32 angleMin, f32 angleMax, f32 srcSize, f32 velScale, AEVec2* pVelInit)
{
	f32 velRange, velMin, scaleRange, scaleMin;

	if (type == PTCL_EXHAUST)
	{
		velRange   = velScale * 30.0f;
		velMin     = velScale * 10.0f;
		scaleRange = 0.0f;
		scaleMin   = 0.4f;

		for (u32 i = 0; i < count; i++)
		{
			f32		t		= AERandFloat() * 2.0f - 1.0f;
			f32		dir		= angleMin + AERandFloat() * (angleMax - angleMin);
			f32		velMag	= velMin + fabs(t) * velRange;
			AEVec2	vel;

			AEVec2Set	(&vel, AECos(dir), AESin(dir));
			AEVec2Scale	(&vel, &vel, velMag);

			if (pVelInit)
				AEVec2Add(&vel, &vel, pVelInit);
			
			gameObjInstCreate(
				(fabs(t) < 0.2f) ? (TYPE_PTCL_YELLOW) : (TYPE_PTCL_RED), 
				t * scaleRange + scaleMin, 
				pPos, &vel, AERandFloat() * 2.0f * PI, false);
		}
	}
	else if ((PTCL_EXPLOSION_S <= type) && (type <= PTCL_EXPLOSION_L))
	{
		if (type == PTCL_EXPLOSION_S)
		{
			velRange   = 50.0f;
			velMin     = 20.0f;
			scaleRange = 0.5f;
			scaleMin   = 0.2f;
		}
		else if (type == PTCL_EXPLOSION_M)
		{
			velRange   = 100.0f;
			velMin     = 50.0f;
			scaleRange = 0.5f;
			scaleMin   = 0.5f;
		}
		else
		{
			velRange   = 150.0f;
			velMin     = 20.0f;
			scaleRange = 1.0f;
			scaleMin   = 0.5f;
		}

		velRange *= velScale;
		velMin   *= velScale;

		for (u32 i = 0; i < count; i++)
		{
			f32		dir		= angleMin + (angleMax - angleMin) * AERandFloat();
			f32		t		= AERandFloat();
			f32		velMag	= t * velRange + velMin;
			AEVec2	vel;
			AEVec2	pos;

			AEVec2Set(&pos, pPos->x + (AERandFloat() - 0.5f) * srcSize, pPos->y + (AERandFloat() - 0.5f) * srcSize);

			AEVec2Set	(&vel, AECos(dir), AESin(dir));
			AEVec2Scale	(&vel, &vel, velMag);

			if (pVelInit)
				AEVec2Add(&vel, &vel, pVelInit);
			
			gameObjInstCreate(
				(t < 0.25f) ? (TYPE_PTCL_WHITE)  : (
				(t < 0.50f) ? (TYPE_PTCL_YELLOW) : (TYPE_PTCL_RED)), 
				t * scaleRange + scaleMin, 
				&pos, &vel, AERandFloat() * 2.0f * PI, false);
		}
	}
}

// ---------------------------------------------------------------------------

GameObjInst* missileAcquireTarget(GameObjInst* pMissile)
{
	AEVec2		 dir, u;
	f32			 uLen, angleMin = AECos(0.25f * PI), minDist = FLT_MAX;
	GameObjInst* pTarget = 0;

	AEVec2Set(&dir, AECos(pMissile->dirCurr), AESin(pMissile->dirCurr));

	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		if (((pInst->flag & FLAG_ACTIVE) == 0) || (pInst->pObject->type != TYPE_ASTEROID))
			continue;

		AEVec2Sub(&u, &pInst->posCurr, &pMissile->posCurr);
		uLen = AEVec2Length(&u);

		if (uLen < 1.0f)
			continue;

		AEVec2Scale(&u, &u, 1.0f / uLen);

		if (AEVec2DotProduct(&dir, &u) < angleMin)
			continue;
		
		if (uLen < minDist)
		{
			minDist = uLen;
			pTarget = pInst;
		}
	}

	return pTarget;
}

// ---------------------------------------------------------------------------



