// ---------------------------------------------------------------------------
// Project Name		:	Asteroid
// File Name		:	GameState_Platform.cpp
// Author			:	Sun Tjen Fam, Samir Abou Samra, Antoine Abi Chakra
// Creation Date	:	2008/03/04
// Purpose			:	Implementation of the platform game state
// History			:
//	- 
// ---------------------------------------------------------------------------


#include "Main.h"

#define GAME_OBJ_NUM_MAX			32
#define GAME_OBJ_INST_NUM_MAX		2048

#define GRAVITY 30.0f

#define FLAG_ACTIVE		0x00000001
#define FLAG_VISIBLE	0x00000002

struct GameObj
{
	u32				type;		// object type
	AEGfxTriList*	pMesh;		// pbject
};

struct GameObjInst
{
	GameObj*		pObject;	// pointer to the 'original'
	u32				flag;		// bit flag or-ed together
	f32				scale;
	AEVec2			posCurr;	// object current position
	AEVec2			velCurr;	// object current velocity
	f32				dirCurr;	// object current direction

	AEMtx33			transform;	// object drawing matrix

	// pointer to custom data specific for each object type
	void*			pUserData;
};

enum TYPE_OBJECT
{
	TYPE_OBJECT_EMPTY,			//0
	TYPE_OBJECT_COLLISION,		//1
	TYPE_OBJECT_HERO,			//2
	TYPE_OBJECT_ENEMY1,			//3
	TYPE_OBJECT_COIN			//4
};

// list of original object
static GameObj			*sGameObjList;
static u32				sGameObjNum;

// list of object instances
static GameObjInst		*sGameObjInstList;
static u32				sGameObjInstNum;

//Binary map data
static int **MapData;
static int **BinaryCollisionArray;
static int BINARY_MAP_WIDTH;
static int BINARY_MAP_HEIGHT;
static GameObjInst *pBlackInstance;
static GameObjInst *pWhiteInstance;
static AEMtx33 MapTransform;
int GetCellValue(int X, int Y);
int CheckInstanceBinaryMapCollision(float PosX, float PosY, float scaleX, float scaleY);
void SnapToCell(float *Coordinate);
int ImportMapDataFromFile(char *FileName);
void FreeMapData(void);


#define	COLLISION_LEFT		0x00000001	//0001
#define	COLLISION_RIGHT		0x00000002	//0010
#define	COLLISION_TOP		0x00000004	//0100
#define	COLLISION_BOTTOM	0x00000008	//1000



// function to create/destroy a game object object
static GameObjInst*	gameObjInstCreate (u32 type, f32 scale, AEVec2* pPos, AEVec2* pVel, f32 dir);
static void			gameObjInstDestroy(GameObjInst* pInst);

//We need a pointer to the hero's instance for input purposes
static GameObjInst *pHero;


void GameStatePlatformLoad(void)
{
	sGameObjList = (GameObj *)calloc(GAME_OBJ_NUM_MAX, sizeof(GameObj));
	sGameObjInstList = (GameObjInst *)calloc(GAME_OBJ_INST_NUM_MAX, sizeof(GameObjInst));
	sGameObjNum = 0;

	GameObj* pObj;

	//Creating the black object
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_OBJECT_EMPTY;

	AEGfxTriStart();

	//1st argument: X
	//2nd argument: Y
	//3rd argument: ARGB
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFF000000, 
		 0.5f,  -0.5f, 0xFF000000, 
		-0.5f,  0.5f, 0xFF000000);
	
	AEGfxTriAdd(
		-0.5f, 0.5f, 0xFF000000, 
		 0.5f,  -0.5f, 0xFF000000, 
		0.5f,  0.5f, 0xFF000000);

	pObj->pMesh = AEGfxTriEnd();

		
	
	//Creating the white object
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_OBJECT_COLLISION;

	AEGfxTriStart();

	//1st argument: X
	//2nd argument: Y
	//3rd argument: ARGB
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 
		 0.5f,  -0.5f, 0xFFFFFFFF, 
		-0.5f,  0.5f, 0xFFFFFFFF);
	
	AEGfxTriAdd(
		-0.5f, 0.5f, 0xFFFFFFFF, 
		 0.5f,  -0.5f, 0xFFFFFFFF, 
		0.5f,  0.5f, 0xFFFFFFFF);

	pObj->pMesh = AEGfxTriEnd();


	//Creating the hero object
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_OBJECT_HERO;

	AEGfxTriStart();

	//1st argument: X
	//2nd argument: Y
	//3rd argument: ARGB
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFF0000FF, 
		 0.5f,  -0.5f, 0xFF0000FF, 
		-0.5f,  0.5f, 0xFF0000FF);
	
	AEGfxTriAdd(
		-0.5f, 0.5f, 0xFF0000FF, 
		 0.5f,  -0.5f, 0xFF0000FF, 
		0.5f,  0.5f, 0xFF0000FF);

	pObj->pMesh = AEGfxTriEnd();


	//Creating the enemey1 object
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_OBJECT_ENEMY1;

	AEGfxTriStart();

	//1st argument: X
	//2nd argument: Y
	//3rd argument: ARGB
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFF00FF00, 
		 0.5f,  -0.5f, 0xFF00FF00, 
		-0.5f,  0.5f, 0xFF00FF00);
	
	AEGfxTriAdd(
		-0.5f, 0.5f, 0xFF00FF00, 
		 0.5f,  -0.5f, 0xFF00FF00, 
		0.5f,  0.5f, 0xFF00FF00);

	pObj->pMesh = AEGfxTriEnd();


	//Creating the Coin object
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_OBJECT_ENEMY1;

	AEGfxTriStart();

	//1st argument: X
	//2nd argument: Y
	//3rd argument: ARGB
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFF00, 
		 0.5f,  -0.5f, 0xFFFFFF00, 
		-0.5f,  0.5f, 0xFFFFFF00);
	
	AEGfxTriAdd(
		-0.5f, 0.5f, 0xFFFFFF00, 
		 0.5f,  -0.5f, 0xFFFFFF00, 
		0.5f,  0.5f, 0xFFFFFF00);

	pObj->pMesh = AEGfxTriEnd();

	//Setting intital binary map values
	MapData = 0;
	BinaryCollisionArray = 0;
	BINARY_MAP_WIDTH = 0;
	BINARY_MAP_HEIGHT = 0;

	//Importing Data
	if(!ImportMapDataFromFile("Exported.txt"))
		gGameStateNext = GS_QUIT;


	//Computing the matrix which take a point out of the normalized coordinates system
	//of the binary map 
	AEMtx33 scale, trans, rot;
	AEMtx33Trans(&trans, -BINARY_MAP_WIDTH/2.0f, -BINARY_MAP_HEIGHT/2.0f);
	AEMtx33Scale(&scale, 4, 3);
	AEMtx33RotDeg(&rot, 0.0f);

	AEMtx33Concat(&MapTransform, &scale, &trans);
	AEMtx33Concat(&MapTransform, &rot, &MapTransform);
}

void GameStatePlatformInit(void)
{
	int i, j;
	//Create an object instance representing the black cell
	pBlackInstance = gameObjInstCreate(TYPE_OBJECT_EMPTY, 1.0f, 0, 0, 0.0f);
	pBlackInstance->flag ^= FLAG_VISIBLE;

	//Create an object instance representing the white cell
	pWhiteInstance = gameObjInstCreate(TYPE_OBJECT_COLLISION, 1.0f, 0, 0, 0.0f);
	pWhiteInstance->flag ^= FLAG_VISIBLE;

	GameObjInst *pInst;
	AEVec2 Pos;
	float scale;
	//creating the main character, the enemies and the coins according to their initial positions in MapData
	for(i = 0; i < BINARY_MAP_WIDTH; ++i)
		for(j = 0; j < BINARY_MAP_HEIGHT; ++j)
		{
			switch(MapData[i][j])
			{
				case TYPE_OBJECT_EMPTY:			//0
					break;
					
				case TYPE_OBJECT_COLLISION:		//1
					break;

				case TYPE_OBJECT_HERO:			//2
					AEVec2Set(&Pos, i + 0.5f, j + 0.5f);
					pHero = gameObjInstCreate(TYPE_OBJECT_HERO, 1.0f, &Pos, 0, 0);
					break;

				case TYPE_OBJECT_ENEMY1:		//3
					AEVec2Set(&Pos, i + 0.5f, j + 0.5f);
					pInst = gameObjInstCreate(TYPE_OBJECT_ENEMY1, 1.0f, &Pos, 0, 0);
					break;

				case TYPE_OBJECT_COIN:			//4
					AEVec2Set(&Pos, i + 0.5f, j + 0.5f);
					pInst = gameObjInstCreate(TYPE_OBJECT_COIN, 1.0f, &Pos, 0, 0);
					break;
			}
		}
}

void GameStatePlatformUpdate(void)
{
	static float f = 0;
		//Computing the matrix which take a point out of the normalized coordinates system
	//of the binary map 
	AEMtx33 scale, trans, rot;
	AEMtx33Trans(&trans, -BINARY_MAP_WIDTH/2.0f, -BINARY_MAP_HEIGHT/2.0f);
	AEMtx33Scale(&scale, 4, 3);
	AEMtx33RotDeg(&rot, f);

	f += 300.0f * gAEFrameTime;

	AEMtx33Concat(&MapTransform, &scale, &trans);
	AEMtx33Concat(&MapTransform, &rot, &MapTransform);
	int i;

	//Handle Input
	if(AEInputCheckCurr(DIK_RIGHT))
		pHero->velCurr.x = 2.0f;
	else
	if(AEInputCheckCurr(DIK_LEFT))
		pHero->velCurr.x = -2.0f;
	//else
	//	pHero->velCurr.x = 0.0f;

	if(AEInputCheckTriggered(DIK_SPACE))
		pHero->velCurr.y = 500.0f*gAEFrameTime;

	if(AEInputCheckCurr(DIK_Q))
		gGameStateNext = GS_QUIT;

	
	//Update object instances behavior
	for(i = 0; i < GAME_OBJ_INST_NUM_MAX; ++i)
	{
		GameObjInst *pInst = sGameObjInstList + i;

		// skip non-active object
		if (0 == (pInst->flag & FLAG_ACTIVE))
			continue;

		AEVec2 gravity;
		AEVec2Set(&gravity, -AESinDeg(f) * GRAVITY * gAEFrameTime, -AECosDeg(f) * GRAVITY * gAEFrameTime);

		if (pInst == pHero)
			PRINT("%f %f\n", gravity.x, gravity.y);

		//pInst->velCurr.y -= GRAVITY*gAEFrameTime;
		AEVec2Add(&pInst->velCurr, &pInst->velCurr, &gravity);
	}

	//Update object instances positions
	for(i = 0; i < GAME_OBJ_INST_NUM_MAX; ++i)
	{
		GameObjInst *pInst = sGameObjInstList + i;

		// skip non-active object
		if (0 == (pInst->flag & FLAG_ACTIVE))
			continue;

		// update the position using: P1 = V1*t + P0
		pInst->posCurr.x = pInst->velCurr.x * gAEFrameTime + pInst->posCurr.x;
		pInst->posCurr.y = pInst->velCurr.y * gAEFrameTime + pInst->posCurr.y;
	}



	//Check for collision
	for(i = 0; i < GAME_OBJ_INST_NUM_MAX; ++i)
	{
		GameObjInst *pInst = sGameObjInstList + i;

		// skip non-active object
		if (0 == (pInst->flag & FLAG_ACTIVE))
			continue;

		if(pInst->pObject->type == TYPE_OBJECT_HERO || pInst->pObject->type == TYPE_OBJECT_ENEMY1)
		{
			int Flag = 0;

			Flag = CheckInstanceBinaryMapCollision(pInst->posCurr.x, pInst->posCurr.y, pInst->scale, pInst->scale);

			if(Flag & COLLISION_BOTTOM || Flag & COLLISION_TOP)
			{
				SnapToCell(&pInst->posCurr.y);
				pInst->velCurr.y = 0;
			}
			
			if(Flag & COLLISION_RIGHT || Flag & COLLISION_LEFT)
			{
				SnapToCell(&pInst->posCurr.x);
				pInst->velCurr.x = 0;
			}	
		}
	}
	
	//Compute the transformation matrices of the game object instances
	for(i = 0; i < GAME_OBJ_INST_NUM_MAX; ++i)
	{
		AEMtx33 scale, rot, trans;
		GameObjInst *pInst = sGameObjInstList + i;

		// skip non-active object
		if (0 == (pInst->flag & FLAG_ACTIVE))
			continue;

		AEMtx33Scale(&scale, pInst->scale, pInst->scale);
		AEMtx33Rot(&rot, pInst->dirCurr);
		AEMtx33Trans(&trans, pInst->posCurr.x, pInst->posCurr.y);

		AEMtx33Concat(&pInst->transform, &trans, &rot);
		AEMtx33Concat(&pInst->transform, &pInst->transform, &scale);
	}
}

void GameStatePlatformDraw(void)
{
	//Drawing the tile map (the grid)
	int i, j;
	AEMtx33 cellTranslation;

	//Drawing the tile map
	for(i = 0; i < BINARY_MAP_WIDTH; ++i)
		for(j = 0; j < BINARY_MAP_HEIGHT; ++j)
		{
			AEMtx33Trans(&cellTranslation, (float)i + 0.5f, (float)j + 0.5f);
			
			//Don't forget the "out of binary map coordinates system" matrix
			AEMtx33Concat(&cellTranslation, &MapTransform, &cellTranslation);

			AEGfxSetTransform(&cellTranslation);

			//if it's a non collision area, draw the black object
			switch(BinaryCollisionArray[i][j])
			{
			case TYPE_OBJECT_EMPTY: //No collision
				AEGfxTriDraw(pBlackInstance->pObject->pMesh);
				break;

			case TYPE_OBJECT_COLLISION: //Collision
				AEGfxTriDraw(pWhiteInstance->pObject->pMesh);
				break;
			}
		}

	//Drawing the object instances
	for (i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		//if(pInst->pObject->type == TYPE_OBJECT_HERO

		// skip non-active object
		if (0 == (pInst->flag & FLAG_ACTIVE) || 0 == (pInst->flag & FLAG_VISIBLE))
			continue;
		
		
		//Don't forget the "out of binary map coordinates system" matrix
		AEMtx33Concat(&sGameObjInstList[i].transform, &MapTransform, &sGameObjInstList[i].transform);
		AEGfxSetTransform	(&sGameObjInstList[i].transform);
		AEGfxTriDraw		(sGameObjInstList[i].pObject->pMesh);
	}

	char strBuffer[100];
	memset(strBuffer, 0, 100*sizeof(char));
	sprintf(strBuffer, "FPS:  %.20f", gAEFrameTime);
	AEGfxPrint(30, 30, -1, strBuffer);
}


void GameStatePlatformFree(void)
{
	// kill all object in the list
	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
		gameObjInstDestroy(sGameObjInstList + i);
}

void GameStatePlatformUnload(void)
{
	FreeMapData();
}


GameObjInst* gameObjInstCreate(u32 type, f32 scale, AEVec2* pPos, AEVec2* pVel, f32 dir)
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
			pInst->flag		 = FLAG_ACTIVE | FLAG_VISIBLE;
			pInst->scale	 = scale;
			pInst->posCurr	 = pPos ? *pPos : zero;
			pInst->velCurr	 = pVel ? *pVel : zero;
			pInst->dirCurr	 = dir;
			pInst->pUserData = 0;
			
			// return the newly created instance
			return pInst;
		}
	}
}

void gameObjInstDestroy(GameObjInst* pInst)
{
	// if instance is destroyed before, just return
	if (pInst->flag == 0)
		return;

	// zero out the flag
	pInst->flag = 0;
}


int GetCellValue(int X, int Y)
{
	//checking if the coordinates X or Y are out of bounds
	if(X < 0 || X >= BINARY_MAP_WIDTH || Y < 0 || Y > BINARY_MAP_HEIGHT)
		return 0;

	return BinaryCollisionArray[X][Y];
}


int CheckInstanceBinaryMapCollision(float PosX, float PosY, float scaleX, float scaleY)
{
	//At the end of this function, "Flag" will be used to determine which sides
	//of the object instance are colliding. 2 hot spots will be placed on each side.
	int Flag, CellID;

	AEVec2 hotSpot;
	Flag = 0;

	//bottom left hot spot
	hotSpot.x = PosX - scaleX/4.0f;
	hotSpot.y = PosY - scaleY/2.0f;

	CellID = GetCellValue((int)hotSpot.x, (int)hotSpot.y);

	if(CellID == TYPE_OBJECT_COLLISION)
		Flag |= COLLISION_BOTTOM;

	//bottom right hot spot
	hotSpot.x = PosX + scaleX/4.0f;
	hotSpot.y = PosY - scaleY/2.0f;

	CellID = GetCellValue((int)hotSpot.x, (int)hotSpot.y);

	if(CellID == TYPE_OBJECT_COLLISION)
		Flag |= COLLISION_BOTTOM;



	//top left hot spot
	hotSpot.x = PosX - scaleX/4.0f;
	hotSpot.y = PosY + scaleY/2.1f;

	CellID = GetCellValue((int)hotSpot.x, (int)hotSpot.y);

	if(CellID == TYPE_OBJECT_COLLISION)
		Flag |= COLLISION_TOP;

	//top right hot spot
	hotSpot.x = PosX + scaleX/4.0f;
	hotSpot.y = PosY + scaleY/2.1f;

	CellID = GetCellValue((int)hotSpot.x, (int)hotSpot.y);

	if(CellID == TYPE_OBJECT_COLLISION)
		Flag |= COLLISION_TOP;



	//right top hot spot
	hotSpot.x = PosX + scaleX/2.0f;
	hotSpot.y = PosY + scaleY/4.0f;

	CellID = GetCellValue((int)hotSpot.x, (int)hotSpot.y);

	if(CellID == TYPE_OBJECT_COLLISION)
		Flag |= COLLISION_RIGHT;


	//right bottom hot spot
	hotSpot.x = PosX + scaleX/2.0f;
	hotSpot.y = PosY - scaleY/4.0f;

	CellID = GetCellValue((int)hotSpot.x, (int)hotSpot.y);

	if(CellID == TYPE_OBJECT_COLLISION)
		Flag |= COLLISION_RIGHT;



	//left top hot spot
	hotSpot.x = PosX - scaleX/2.0f;
	hotSpot.y = PosY + scaleY/4.0f;

	CellID = GetCellValue((int)hotSpot.x, (int)hotSpot.y);

	if(CellID == TYPE_OBJECT_COLLISION)
		Flag |= COLLISION_LEFT;


	//left bottom hot spot
	hotSpot.x = PosX - scaleX/2.0f;
	hotSpot.y = PosY - scaleY/4.0f;

	CellID = GetCellValue((int)hotSpot.x, (int)hotSpot.y);

	if(CellID == TYPE_OBJECT_COLLISION)
		Flag |= COLLISION_LEFT;

	return Flag;
}


void SnapToCell(float *Coordinate)
{
	*Coordinate = (int)(*Coordinate) + 0.5f;
}

int ImportMapDataFromFile(char *FileName)
{
	FILE *fp;
	char tmpStr[20];
	int Value;
	int i, j;

	memset(tmpStr, 0, 20*sizeof(char));

	if(!FileName)
		return 0;

	fp = fopen(FileName, "r");
	
	if(!fp)
		return 0;

	//Reading the width size
	fscanf(fp, "%s %i", tmpStr, &BINARY_MAP_WIDTH);
	//Reading the height size
	fscanf(fp, "%s %i", tmpStr, &BINARY_MAP_HEIGHT);

	MapData = (int **)calloc(BINARY_MAP_WIDTH, sizeof(int *));
	for(i = 0; i < BINARY_MAP_WIDTH; ++i)
		MapData[i] = (int *)calloc(BINARY_MAP_HEIGHT, sizeof(int));

	BinaryCollisionArray = (int **)calloc(BINARY_MAP_WIDTH, sizeof(int *));

	for(i = 0; i < BINARY_MAP_WIDTH; ++i)
		BinaryCollisionArray[i] = (int *)calloc(BINARY_MAP_HEIGHT, sizeof(int));


	for(j = 0; j < BINARY_MAP_HEIGHT; ++j)
	{
		for(i = 0; i < BINARY_MAP_WIDTH; ++i)
		{
			fscanf(fp, "%i", &Value);
			//copy the map data as is
			MapData[i][j] = Value;

			//copy the collision data. Anything but 1 is a non collision area
			if(1 == Value)
				BinaryCollisionArray[i][j] = 1;
			else
				BinaryCollisionArray[i][j] = 0;
		}
	}

	fclose(fp);
	return 1;
}


void FreeMapData(void)
{
	int i;

	for(i = 0; i < BINARY_MAP_WIDTH; ++i)
		free(MapData[i]);

	free(MapData);


	for(i = 0; i < BINARY_MAP_WIDTH; ++i)
		free(BinaryCollisionArray[i]);

	free(BinaryCollisionArray);
}