#include "main.h"
#include "GameState_BasePlay.h"

GameState_BasePlay::GameState_BasePlay()
{}

GameState_BasePlay::~GameState_BasePlay()
{}

void GameState_BasePlay::load(void)
{
	// load/create the mesh data
	loadGameObjList();
}

// ---------------------------------------------------------------------------

void GameState_BasePlay::init(void)
{
	// reset the number of current asteroid and the total allowed
	m_game.m_asteroids = 0;
	m_game.m_maxAsteroids = AST_NUM_MIN;

	// create the main ship
	m_game.m_localShip = m_game.gameObjInstCreate(TYPE_SHIP, SHIP_SIZE, 0, 0, 0.0f, true);
	AE_ASSERT(m_game.m_localShip);

	// get the time the asteroid is created
	m_game.m_asteroidTimer = AEGetTime();
	
	// generate the initial asteroid
	//for (u32 i = 0; i < m_game.m_maxAsteroids; i++)
	//	m_game.astCreate(0);

	// reset the score and the number of ship
	m_game.m_score = 0;
	m_game.m_lives = SHIP_INITIAL_NUM;
	m_game.m_specials = SHIP_SPECIAL_NUM;

	// reset the delay to switch to the result state after game over
	m_game.m_gameTimer = 2.0f;
}

// ---------------------------------------------------------------------------

void GameState_BasePlay::update(void)
{
	// =================
	// update the input
	// =================

	if ( !m_game.m_localShip && (m_game.m_gameTimer -= (f32)(gAEFrameTime)) < 0.0)
		this->onEnd();
	else
	{
		if (AEInputCheckCurr(DIK_UP))
		{
			AEVec2 pos, dir;

			AEVec2Set	(&dir, AECos(m_game.m_localShip->dirCurr), AESin(m_game.m_localShip->dirCurr));
			pos = dir;
			AEVec2Scale	(&dir, &dir, SHIP_ACCEL_FORWARD * (f32)(gAEFrameTime));
			AEVec2Add	(&m_game.m_localShip->velCurr, &m_game.m_localShip->velCurr, &dir);
			AEVec2Scale	(&m_game.m_localShip->velCurr, &m_game.m_localShip->velCurr, pow(SHIP_DAMP_FORWARD, (f32)(gAEFrameTime)));

			AEVec2Scale	(&pos, &pos, -m_game.m_localShip->scale);
			AEVec2Add	(&pos, &pos, &m_game.m_localShip->posCurr);

			m_game.sparkCreate(PTCL_EXHAUST, &pos, 2, m_game.m_localShip->dirCurr + 0.8f * PI, m_game.m_localShip->dirCurr + 1.2f * PI);
		}
		if (AEInputCheckCurr(DIK_DOWN))
		{
			AEVec2 dir;
			
			AEVec2Set	(&dir, AECos(m_game.m_localShip->dirCurr), AESin(m_game.m_localShip->dirCurr));
			AEVec2Scale	(&dir, &dir, SHIP_ACCEL_BACKWARD * (f32)(gAEFrameTime));
			AEVec2Add	(&m_game.m_localShip->velCurr, &m_game.m_localShip->velCurr, &dir);
			AEVec2Scale	(&m_game.m_localShip->velCurr, &m_game.m_localShip->velCurr, pow(SHIP_DAMP_BACKWARD, (f32)(gAEFrameTime)));
		}
		if (AEInputCheckCurr(DIK_LEFT))
		{
			m_game.m_shipRotationSpeed   += (SHIP_ROT_SPEED - m_game.m_shipRotationSpeed) * 0.1f;
			m_game.m_localShip->dirCurr += m_game.m_shipRotationSpeed * (f32)(gAEFrameTime);
			m_game.m_localShip->dirCurr =  AEWrap(m_game.m_localShip->dirCurr, -PI, PI);
		}
		else if (AEInputCheckCurr(DIK_RIGHT))
		{
			m_game.m_shipRotationSpeed   += (SHIP_ROT_SPEED - m_game.m_shipRotationSpeed) * 0.1f;
			m_game.m_localShip->dirCurr -= m_game.m_shipRotationSpeed * (f32)(gAEFrameTime);
			m_game.m_localShip->dirCurr =  AEWrap(m_game.m_localShip->dirCurr, -PI, PI);
		}
		else
		{
			m_game.m_shipRotationSpeed = 0.0f;
		}
		if (AEInputCheckTriggered(DIK_SPACE))
		{
			AEVec2 vel;

			AEVec2Set	(&vel, AECos(m_game.m_localShip->dirCurr), AESin(m_game.m_localShip->dirCurr));
			AEVec2Scale	(&vel, &vel, BULLET_SPEED);
			
			m_game.gameObjInstCreate(TYPE_BULLET, 1.0f, &m_game.m_localShip->posCurr, &vel, m_game.m_localShip->dirCurr, true);
		}
		if (AEInputCheckTriggered(DIK_Z) && (m_game.m_specials >= BOMB_COST))
		{
			u32 i;

			// make sure there is no bomb is active currently
			for (i = 0; i < m_game.m_gameObjInsts.size(); i++)
				if ((m_game.m_gameObjInsts[i].flag & FLAG_ACTIVE) && (m_game.m_gameObjInsts[i].pObject->type == TYPE_BOMB))
					break;

			// if no bomb is active currently, create one
			if (i == m_game.m_gameObjInsts.size())
			{
				m_game.m_specials -= BOMB_COST;
				m_game.gameObjInstCreate(TYPE_BOMB, 1.0f, &m_game.m_localShip->posCurr, 0, 0, true);
			}
		}
		if (AEInputCheckTriggered(DIK_X) && (m_game.m_specials >= MISSILE_COST))
		{
			m_game.m_specials -= MISSILE_COST;

			f32			 dir = m_game.m_localShip->dirCurr;
			AEVec2       vel = m_game.m_localShip->velCurr;
			AEVec2		 pos;

			AEVec2Set	(&pos, AECos(m_game.m_localShip->dirCurr), AESin(m_game.m_localShip->dirCurr));
			AEVec2Scale	(&pos, &pos, m_game.m_localShip->scale * 0.5f);
			AEVec2Add	(&pos, &pos, &m_game.m_localShip->posCurr);
			
			m_game.gameObjInstCreate(TYPE_MISSILE, 1.0f, &pos, &vel, dir, true);
		}
	}

	// ==================================
	// create new asteroids if necessary
	// ==================================

	if ((m_game.m_asteroids < m_game.m_maxAsteroids) && ((AEGetTime() - m_game.m_asteroidTimer) > AST_CREATE_DELAY))
	{
		// keep track the last time an asteroid is created
		m_game.m_asteroidTimer = AEGetTime();

		// create an asteroid
		//m_game.astCreate(0);
	}

	// ===============
	// update physics
	// ===============

	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = &m_game.m_gameObjInsts[i];

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
		GameObjInst* pInst = &m_game.m_gameObjInsts[i];

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
			if (m_game.m_localShip)
			{
				// apply acceleration propotional to the distance from the asteroid to the ship
				AEVec2Sub	(&u, &m_game.m_localShip->posCurr, &pInst->posCurr);
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
				m_game.gameObjInstDestroy(pInst);
		}
		// check if the object is a bomb
		else if (pInst->pObject->type == TYPE_BOMB)
		{
			// adjust the life counter
			pInst->life -= (f32)(gAEFrameTime) / BOMB_LIFE;
			
			if (pInst->life < 0.0f)
			{
				m_game.gameObjInstDestroy(pInst);
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
					m_game.sparkCreate(PTCL_EXHAUST, &u, 1, dir + 0.40f * PI, dir + 0.60f * PI);
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
				m_game.gameObjInstDestroy(pInst);
			}
			else
			{
				AEVec2 dir;

				if (pInst->pUserData == 0)
				{
					pInst->pUserData = m_game.missileAcquireTarget(pInst);
				}
				else
				{
					GameObjInst* pTarget = (GameObjInst*)(pInst->pUserData);

					// if the target is no longer valid, reacquire
					if (((pTarget->flag & FLAG_ACTIVE) == 0) || (pTarget->pObject->type != TYPE_ASTEROID))
						pInst->pUserData = m_game.missileAcquireTarget(pInst);
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

				m_game.sparkCreate(PTCL_EXHAUST, &pInst->posCurr, 1, pInst->dirCurr + 0.8f * PI, pInst->dirCurr + 1.2f * PI);
			}
		}
		// check if the object is a particle
		else if ((TYPE_PTCL_WHITE <= pInst->pObject->type) && (pInst->pObject->type <= TYPE_PTCL_RED))
		{
			pInst->scale   *= pow(PTCL_SCALE_DAMP, (f32)(gAEFrameTime));
			pInst->dirCurr += 0.1f;
			AEVec2Scale(&pInst->velCurr, &pInst->velCurr, pow(PTCL_VEL_DAMP, (f32)(gAEFrameTime)));

			if (pInst->scale < PTCL_SCALE_DAMP)
				m_game.gameObjInstDestroy(pInst);
		}
	}

	// ====================
	// check for collision
	// ====================

	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pSrc = &m_game.m_gameObjInsts[i];

		// skip non-active object
		if ((pSrc->flag & FLAG_ACTIVE) == 0)
			continue;

		if ((pSrc->pObject->type == TYPE_BULLET) || (pSrc->pObject->type == TYPE_MISSILE))
		{
			for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
			{
				GameObjInst* pDst = &m_game.m_gameObjInsts[j];

				// skip no-active and non-asteroid object
				if (((pDst->flag & FLAG_ACTIVE) == 0) || (pDst->pObject->type != TYPE_ASTEROID))
					continue;

				if (AETestPointToRect(&pSrc->posCurr, &pDst->posCurr, pDst->scale, pDst->scale) == false)
					continue;
				
				if (pDst->scale < AST_SIZE_MIN)
				{
					m_game.sparkCreate(PTCL_EXPLOSION_M, &pDst->posCurr, (u32)(pDst->scale * 10), pSrc->dirCurr - 0.05f * PI, pSrc->dirCurr + 0.05f * PI, pDst->scale);
					m_game.m_score++;

					if ((m_game.m_score % AST_SPECIAL_RATIO) == 0)
						m_game.m_specials++;
					if ((m_game.m_score % AST_SHIP_RATIO) == 0)
						m_game.m_lives++;
					if (m_game.m_score == m_game.m_maxAsteroids * 5)
						m_game.m_maxAsteroids = (m_game.m_maxAsteroids < AST_NUM_MAX) ? (m_game.m_maxAsteroids * 2) : m_game.m_maxAsteroids;

					// destroy the asteroid
					m_game.gameObjInstDestroy(pDst);
				}
				else
				{
					m_game.sparkCreate(PTCL_EXPLOSION_S, &pSrc->posCurr, 10, pSrc->dirCurr + 0.9f * PI, pSrc->dirCurr + 1.1f * PI);

					// impart some of the bullet/missile velocity to the asteroid
					AEVec2Scale(&pSrc->velCurr, &pSrc->velCurr, 0.01f * (1.0f - pDst->scale / AST_SIZE_MAX));
					AEVec2Add  (&pDst->velCurr, &pDst->velCurr, &pSrc->velCurr);

					// split the asteroid to 4
					if ((pSrc->pObject->type == TYPE_MISSILE) || ((pDst->life -= 1.0f) < 0.0f))
						m_game.astCreate(pDst);
				}

				// destroy the bullet
				m_game.gameObjInstDestroy(pSrc);

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
				GameObjInst* pDst = &m_game.m_gameObjInsts[j];

				if (((pDst->flag & FLAG_ACTIVE) == 0) || (pDst->pObject->type != TYPE_ASTEROID))
					continue;

				if (AECalcDistPointToRect(&pSrc->posCurr, &pDst->posCurr, pDst->scale, pDst->scale) > radius)
					continue;

				if (pDst->scale < AST_SIZE_MIN)
				{
					f32 dir = atan2f(pDst->posCurr.y - pSrc->posCurr.y, pDst->posCurr.x - pSrc->posCurr.x);

					m_game.gameObjInstDestroy(pDst);
					m_game.sparkCreate(PTCL_EXPLOSION_M, &pDst->posCurr, 20, dir + 0.4f * PI, dir + 0.45f * PI);
					m_game.m_score++;

					if ((m_game.m_score % AST_SPECIAL_RATIO) == 0)
						m_game.m_specials++;
					if ((m_game.m_score % AST_SHIP_RATIO) == 0)
						m_game.m_lives++;
					if (m_game.m_score == m_game.m_maxAsteroids * 5)
						m_game.m_maxAsteroids = (m_game.m_maxAsteroids < AST_NUM_MAX) ? (m_game.m_maxAsteroids * 2) : m_game.m_maxAsteroids;
				}
				else
				{
					// split the asteroid to 4
					m_game.astCreate(pDst);
				}
			}
		}
		else if (pSrc->pObject->type == TYPE_ASTEROID)
		{
			for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
			{
				GameObjInst* pDst = &m_game.m_gameObjInsts[j];
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
				m_game.resolveCollision(pSrc, pDst, &nrm);
			}
		}
		else if (pSrc->pObject->type == TYPE_SHIP)
		{
			for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
			{
				GameObjInst* pDst = &m_game.m_gameObjInsts[j];

				// skip no-active and non-asteroid object
				if ((pSrc == pDst) || ((pDst->flag & FLAG_ACTIVE) == 0) || (pDst->pObject->type != TYPE_ASTEROID))
					continue;

				// check if the object rectangle overlap
				if (AETestRectToRect(
					&pSrc->posCurr, pSrc->scale, pSrc->scale, 
					&pDst->posCurr, pDst->scale, pDst->scale) == false)
					continue;

				// create the big explosion
				m_game.sparkCreate(PTCL_EXPLOSION_L, &pSrc->posCurr, 100, 0.0f, 2.0f * PI);
				
				// reset the ship position and direction
				AEVec2Zero(&m_game.m_localShip->posCurr);
				AEVec2Zero(&m_game.m_localShip->velCurr);
				m_game.m_localShip->dirCurr = 0.0f;

				m_game.m_specials = SHIP_SPECIAL_NUM;

				// destroy all asteroid near the ship so that you do not die as soon as the ship reappear
				for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
				{
					GameObjInst* pInst = &m_game.m_gameObjInsts[j];
					AEVec2		 u;

					// skip no-active and non-asteroid object
					if (((pInst->flag & FLAG_ACTIVE) == 0) || (pInst->pObject->type != TYPE_ASTEROID))
						continue;

					AEVec2Sub(&u, &pInst->posCurr, &m_game.m_localShip->posCurr);

					if (AEVec2Length(&u) < (m_game.m_localShip->scale * 10.0f))
					{
						m_game.sparkCreate(PTCL_EXPLOSION_M, &pInst->posCurr, 10, -PI, PI);
						m_game.gameObjInstDestroy(pInst);
					}
				}

				// reduce the ship counter
				m_game.m_lives--;
				
				// if counter is less than 0, game over
				if (m_game.m_lives < 0)
				{
					m_game.m_gameTimer = 2.0;
					m_game.gameObjInstDestroy(m_game.m_localShip);
					m_game.m_localShip = 0;
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
		GameObjInst* pInst = &m_game.m_gameObjInsts[i];
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

void GameState_BasePlay::draw(void)
{
	s8 strBuffer[1024];

	// draw all object in the list
	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = &m_game.m_gameObjInsts[i];

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		
		AEGfxSetTransform	(&pInst->transform);
		AEGfxTriDraw		(pInst->pObject->pMesh);
	}

	sprintf(strBuffer, "Score: %d", m_game.m_score);
	AEGfxPrint(10, 10, -1, strBuffer);

	sprintf(strBuffer, "Level: %d", AELogBase2(m_game.m_maxAsteroids));
	AEGfxPrint(10, 30, -1, strBuffer);

	sprintf(strBuffer, "Ship Left: %d", m_game.m_lives >= 0 ? m_game.m_lives : 0);
	AEGfxPrint(600, 10, -1, strBuffer);

	sprintf(strBuffer, "Special:   %d", m_game.m_specials);
	AEGfxPrint(600, 30, -1, strBuffer);

	// display the game over message
	if (m_game.m_lives < 0)
		AEGfxPrint(280, 260, 0xFFFFFFFF, "       GAME OVER       ");
}

// ---------------------------------------------------------------------------

void GameState_BasePlay::free(void)
{
	// kill all object in the list
	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
		m_game.gameObjInstDestroy(&m_game.m_gameObjInsts[i]);
}

// ---------------------------------------------------------------------------

void GameState_BasePlay::unload(void)
{
	// free all mesh
	for (u32 i = 0; i < m_game.m_gameObjects.size(); i++)
		AEGfxTriFree(m_game.m_gameObjects[i].pMesh);
}

// ---------------------------------------------------------------------------
// Static function implementations

void GameState_BasePlay::loadGameObjList()
{
	GameObj* pObj;

	m_game.m_gameObjects.resize(16);
	unsigned counter = 0;
	// ================
	// create the ship
	// ================

	pObj		= &m_game.m_gameObjects[counter++];
	pObj->type	= TYPE_SHIP;

	AEGfxTriStart();
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFF00FF00, 
		 0.5f,  0.0f, 0xFFFFFFFF, 
		-0.5f,  0.5f, 0xFF00FF00);

	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "fail to create object!!");

	// ================
	// create the networked ship
	// ================

	pObj		= &m_game.m_gameObjects[counter++];
	pObj->type	= TYPE_NET_SHIP;

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

	pObj		= &m_game.m_gameObjects[counter++];
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

	pObj		= &m_game.m_gameObjects[counter++];
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

	pObj		= &m_game.m_gameObjects[counter++];
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

	pObj		= &m_game.m_gameObjects[counter++];
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

	pObj		= &m_game.m_gameObjects[counter++];
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

		pObj		= &m_game.m_gameObjects[counter++];
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
