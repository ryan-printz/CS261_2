// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEVec2.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	header file for the 2D vector library
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AE_VEC2_H
#define AE_VEC2_H

// ---------------------------------------------------------------------------

struct AEVec2
{
	f32 x, y;
};

// ---------------------------------------------------------------------------

void	AEVec2Zero				(AEVec2* pResult);
void	AEVec2Set				(AEVec2* pResult, f32 x, f32 y);

void	AEVec2Neg				(AEVec2* pResult, AEVec2* pVec0);
void	AEVec2Add				(AEVec2* pResult, AEVec2* pVec0, AEVec2* pVec1);
void	AEVec2Sub				(AEVec2* pResult, AEVec2* pVec0, AEVec2* pVec1);
void	AEVec2Normalize			(AEVec2* pResult, AEVec2* pVec0);

void	AEVec2Scale				(AEVec2* pResult, AEVec2* pVec0, f32 c);
void	AEVec2ScaleAdd			(AEVec2* pResult, AEVec2* pVec0, AEVec2* pVec1, f32 s);
void	AEVec2ScaleSub			(AEVec2* pResult, AEVec2* pVec0, AEVec2* pVec1, f32 s);

void	AEVec2Project			(AEVec2* pResult, AEVec2* pVec0, AEVec2* pVec1);
void	AEVec2ProjectPerp		(AEVec2* pResult, AEVec2* pVec0, AEVec2* pVec1);

void	AEVec2Lerp				(AEVec2* pResult, AEVec2* pVec0, AEVec2* pVec1, float t);

f32		AEVec2Length			(AEVec2* pVec0);
f32		AEVec2SquareLength		(AEVec2* pVec0);
f32		AEVec2Distance			(AEVec2* pVec0, AEVec2* pVec1);
f32		AEVec2SquareDistance	(AEVec2* pVec0, AEVec2* pVec1);

f32		AEVec2DotProduct		(AEVec2* pVec0, AEVec2* pVec1);
f32		AEVec2CrossProductMag	(AEVec2* pVec0, AEVec2* pVec1);

// ---------------------------------------------------------------------------

#endif // VEC2_H