#pragma once

#include "main.h"

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