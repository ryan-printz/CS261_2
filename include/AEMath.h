// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEMath.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	main header file for the math library
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AE_MATH_H
#define AE_MATH_H

// ---------------------------------------------------------------------------

#include "AEVec2.h"
#include "AEMtx33.h"

// ---------------------------------------------------------------------------

#ifndef EPSILON
	#define	EPSILON	0.00001f
#endif

#ifndef PI
	#define	PI		3.1415926535897932384626433832795f
#endif

#define	HALF_PI	(PI * 0.5f)
#define	TWO_PI	(PI * 2.0f)

// ---------------------------------------------------------------------------

void MathInit();

// ---------------------------------------------------------------------------

f32 AEDegToRad(f32 x);
f32 AERadToDeg(f32 x);

f32 AESin	(f32 x);
f32 AECos	(f32 x);
f32 AETan	(f32 x);
f32 AEASin	(f32 x);
f32 AEACos	(f32 x);
f32 AEATan	(f32 x);

#define AESinDeg(x)		AESin(AEDegToRad(x))
#define AECosDeg(x)		AECos(AEDegToRad(x))
#define AETanDeg(x)		AETan(AEDegToRad(x))
#define AEASinDeg(x)	AERadToDeg(AEASin(x))
#define AEACosDeg(x)	AERadToDeg(AEACos(x))
#define AEATanDeg(x)	AERadToDeg(AEATan(x))

// ---------------------------------------------------------------------------

u32		AEIsPowOf2	(u32 x);
u32		AENextPowOf2(u32 x);
u32		AELogBase2	(u32 x);

f32		AEClamp		(f32 x, f32 x0, f32 x1);
f32		AEWrap		(f32 x, f32 x0, f32 x1);
f32		AEMin		(f32 x, f32 y);
f32		AEMax		(f32 x, f32 y);
bool	AEInRange	(f32 x, f32 x0, f32 x1);

// ---------------------------------------------------------------------------

f32 AECalcDistPointToCircle		(AEVec2* pPos, AEVec2* pCtr, f32 radius);
f32 AECalcDistPointToRect		(AEVec2* pPos, AEVec2* pRect, f32 sizeX, f32 sizeY);
f32 AECalcDistPointToLineSeg	(AEVec2* pPos, AEVec2* pLine0, AEVec2* pLine1);
f32 AECalcDistPointToConvexPoly	(AEVec2* pPos, AEVec2* pVtx, u32 vtxNum);

f32 AECalcDistCircleToCircle	(AEVec2* pCtr0, f32 radius0, AEVec2* pCtr1, f32 radius1);
f32 AECalcDistCircleToRect		(AEVec2* pCtr0, f32 radius0, AEVec2* pRect0, f32 sizeX0, f32 sizeY0);
f32 AECalcDistCircleToLineSeg	(AEVec2* pPos, AEVec2* pLine0, AEVec2* pLine1);
f32 AECalcDistCircleToConvexPoly(AEVec2* pPos, AEVec2* pVtx, u32 vtxNum);

f32 AECalcDistRectToRect	(AEVec2* pRect0, f32 sizeX0, f32 sizeY0, AEVec2* pRect1, f32 sizeX1, f32 sizeY1, AEVec2* pNormal);

// ---------------------------------------------------------------------------

bool AETestPointToCircle	(AEVec2* pPos, AEVec2* pCtr, f32 radius);
bool AETestPointToRect		(AEVec2* pPos, AEVec2* pRect, f32 sizeX, f32 sizeY);

bool AETestCircleToCircle	(AEVec2* pCtr0, f32 radius0, AEVec2* pCtr1, f32 radius1);
bool AETestCircleToRect		(AEVec2* pCtr0, f32 radius0, AEVec2* pRect0, f32 sizeX0, f32 sizeY0);

bool AETestRectToRect		(AEVec2* pRect0, f32 sizeX0, f32 sizeY0, AEVec2* pRect1, f32 sizeX1, f32 sizeY1);

// ---------------------------------------------------------------------------

// sweep a circle with radius 'radius' from ctr0 -> ctr1 againts a point
// * return -ve if circle does not touch the point at any time
f32 AESweepCircleToPoint	(AEVec2* pCtr0, AEVec2* pCtr1, f32 radius, AEVec2* pP);

// sweep a circle with radius 'radius' from ctr0 -> ctr1 againts a line segment
// * return -ve if circle does not intersect the line segment at any time
f32 AESweepCircleToLineSeg	(AEVec2* pCtr0, AEVec2* pCtr1, f32 radius, AEVec2* pP0, AEVec2* pP1, AEVec2* pN = 0);


//TO TEST
//Sweeps a moving point against a static line
f32 AESweepPointToLine		(AEVec2 *pPos, AEVec2 *pVel, AEVec2 *pPnt, AEVec2 *pDirection);

//TO TEST
//Sweeps a moving circle against a static line
f32 AESweepCircleToLine		(AEVec2 *pCtr, f32 radius, AEVec2 *pVel, AEVec2 *pPnt, AEVec2 *pDirection);

//TO TEST
//Reflects a moving point on a static line. Returns 0 if there is no
//collision between the point and the line.
int AEReflectPointOnLine	(AEVec2 *pPos, AEVec2 *pVel, AEVec2 *pPnt, AEVec2 *pDirection, AEVec2 *pNewPosition, AEVec2 *pNewVelocity);


//TO TEST
//Reflects a moving circle on a static line. Returns 0 if there is no
//collision between the circle and the line.
int AEReflectCircleOnLine	(AEVec2 *pCtr, f32 radius, AEVec2 *pVel, AEVec2 *pPnt, AEVec2 *pDirection, AEVec2 *pNewPosition, AEVec2 *newVelocity);


// ---------------------------------------------------------------------------

#endif // AE_MATH_H
