// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEGraphics.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/30
// Purpose			:	Graphics component header file
// History			:
// - 2008/01/30		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AE_GRAPHICS_H
#define AE_GRAPHICS_H

// ---------------------------------------------------------------------------
// Defines

// maximum number of triangles per mesh
#define AE_GFX_TRI_NUM_MAX	8192

enum AEGfxBlendMode
{
	AE_GFX_BM_NONE = 0, 
	AE_GFX_BM_BLEND,  
	AE_GFX_BM_ADD,  

	AE_GFX_BM_NUM
};

// ---------------------------------------------------------------------------
// Struct/Class definitions

struct AEGfxTriList
{
	LPDIRECT3DVERTEXBUFFER9 vtxBuffer;
	u32						vtxNum;
};

// ---------------------------------------------------------------------------
// Extern variables

extern f32	gAEWinMinX; 
extern f32	gAEWinMinY; 
extern f32	gAEWinMaxX; 
extern f32	gAEWinMaxY;

// ---------------------------------------------------------------------------
// Function prototypes

bool AEGfxInit();
void AEGfxReset();
void AEGfxUpdate();
void AEGfxExit();

void AEGfxStart();
void AEGfxEnd();

// function to set the current transformation matrix to use
void AEGfxSetTransform(AEMtx33* pMtx);

void			AEGfxSetBlendMode(AEGfxBlendMode blendMode);
AEGfxBlendMode  AEGfxGetBlendMode();

// functions to create a mesh
void			AEGfxTriStart	();
void			AEGfxTriAdd		(f32 x0, f32 y0, u32 c0, f32 x1, f32 y1, u32 c1, f32 x2, f32 y2, u32 c2);
void			AEGfxTriAdd		(f32 x0, f32 y0, f32 z0, u32 c0, 
								 f32 x1, f32 y1, f32 z1, u32 c1, 
								 f32 x2, f32 y2, f32 z2, u32 c2);
AEGfxTriList*	AEGfxTriEnd		();
void			AEGfxTriDraw	(AEGfxTriList* pTriList);
void			AEGfxTriFree	(AEGfxTriList* pTriList);

// functions to draw simple primitives
// * these are slow!!! use with caution!!
// * DO NOT call any of these function while building mesh using above functions
void			AEGfxPoint		(f32 x0, f32 y0, f32 z0, u32 c0);
void			AEGfxLine		(f32 x0, f32 y0, f32 z0, u32 c0, f32 x1, f32 y1, f32 z1, u32 c1);
void			AEGfxTri		(f32 x0, f32 y0, f32 z0, u32 c0, 
								 f32 x1, f32 y1, f32 z1, u32 c1, 
								 f32 x2, f32 y2, f32 z2, u32 c2);
void			AEGfxQuad		(f32 x0, f32 y0, f32 z0, u32 c0, 
								 f32 x1, f32 y1, f32 z1, u32 c1, 
								 f32 x2, f32 y2, f32 z2, u32 c2, 
								 f32 x3, f32 y3, f32 z3, u32 c3);
void			AEGfxBox		(f32 x0, f32 y0, f32 z0, f32 sizeX, f32 sizeY, f32 sizeZ, u32 c0 = 0xFFFFFFFF,u32 c1 = 0xFF808080);
void			AEGfxSphere		(f32 x0, f32 y0, f32 z0, f32 radius, u32 c0 = 0xFFFFFFFF, u32 c1 = 0xFF808080, u32 division = 8);
void			AEGfxCone		(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, f32 radius, u32 c0 = 0xFF808080, u32 c1 = 0xFFFFFFFF, u32 division = 8);
void			AEGfxAxis		(f32 scale);

u32				AEGfxColInterp	(u32 c0, u32 c1, f32 t);

// font related functions
void			AEGfxPrint		(s32 x, s32 y, u32 color, s8* pStr);

// ---------------------------------------------------------------------------

#endif // AE_GRAPHICS_H

