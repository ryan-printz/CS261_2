#include "Asteroids.h"

Asteroids::Asteroids()
{
	m_gameObjInsts.resize(GAME_OBJ_INST_NUM_MAX);
}

GameObjInst* Asteroids::gameObjInstCreate(unsigned type, float scale, AEVec2* pPos, AEVec2* pVel, float dir, bool forceCreate)
{
	AEVec2 zero = { 0.0f, 0.0f };

	//AE_ASSERT_PARM(type < sGameObjNum);
	
	// loop through the object instance list to find a non-used object instance
	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = &m_gameObjInsts[i];

		// check if current instance is not used
		if (pInst->flag == 0)
		{
			// it is not used => use it to create the new instance
			pInst->pObject	 = &m_gameObjects[type];
			pInst->flag		 = FLAG_ACTIVE;
			pInst->life		 = 1.0f;
			pInst->scale	 = scale;
			pInst->posCurr	 = pPos ? *pPos : zero;
			pInst->velCurr	 = pVel ? *pVel : zero;
			pInst->dirCurr	 = dir;
			pInst->pUserData = 0;

			// keep track the number of asteroid
			if (pInst->pObject->type == TYPE_ASTEROID)
				m_asteroids++;
			
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
			GameObjInst* pInst = &m_gameObjInsts[i];

			// check if current instance is a red particle
			if ((TYPE_PTCL_RED <= pInst->pObject->type) && (pInst->pObject->type <= TYPE_PTCL_WHITE) && (pInst->scale < scaleMin))
			{
				scaleMin = pInst->scale;
				pDst     = pInst;
			}
		}

		if (pDst)
		{
			pDst->pObject	 = &m_gameObjects[type];
			pDst->flag		 = FLAG_ACTIVE;
			pDst->life		 = 1.0f;
			pDst->scale	 = scale;
			pDst->posCurr	 = pPos ? *pPos : zero;
			pDst->velCurr	 = pVel ? *pVel : zero;
			pDst->dirCurr	 = dir;
			pDst->pUserData = 0;

			// keep track the number of asteroid
			if (pDst->pObject->type == TYPE_ASTEROID)
				m_asteroids++;
			
			// return the newly created instance
			return pDst;
		}
	}

	// cannot find empty slot => return 0
	return 0;
}

// ---------------------------------------------------------------------------

void Asteroids::gameObjInstDestroy(GameObjInst* pInst)
{
	// if instance is destroyed before, just return
	if (pInst->flag == 0)
		return;

	// zero out the flag
	pInst->flag = 0;
	
	// keep track the number of asteroid
	if (pInst->pObject->type == TYPE_ASTEROID)
		m_asteroids--;
}

// ---------------------------------------------------------------------------

GameObjInst* Asteroids::astCreate(GameObjInst* pSrc)
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

void Asteroids::resolveCollision(GameObjInst* pSrc, GameObjInst* pDst, AEVec2* pNrm)
{
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
}

// ---------------------------------------------------------------------------

void Asteroids::sparkCreate(u32 type, AEVec2* pPos, u32 count, f32 angleMin, f32 angleMax, f32 srcSize, f32 velScale, AEVec2* pVelInit)
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

GameObjInst* Asteroids::missileAcquireTarget(GameObjInst* pMissile)
{
	AEVec2		 dir, u;
	f32			 uLen, angleMin = AECos(0.25f * PI), minDist = FLT_MAX;
	GameObjInst* pTarget = 0;

	AEVec2Set(&dir, AECos(pMissile->dirCurr), AESin(pMissile->dirCurr));

	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = &m_gameObjInsts[i];

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



