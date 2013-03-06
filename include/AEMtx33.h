// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEMtx33.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	header file for the 3x3 matrix library
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AE_MTX33_H
#define AE_MTX33_H

// ---------------------------------------------------------------------------
// Defines

#define AEMtx33RowCol(pMtx, row, col) (pMtx)->m[(row)][(col)]

// ---------------------------------------------------------------------------
// Struct/Class definitions

// matrix is stored in column major format (ie. the translation term is in the right most column)
struct AEMtx33
{
	f32	m[3][3];
};

// ---------------------------------------------------------------------------

void 		AEMtx33Identity			(AEMtx33* pResult);

void 		AEMtx33Transpose		(AEMtx33* pResult, AEMtx33* pMtx);
f32			AEMtx33Inverse			(AEMtx33* pResult, AEMtx33* pMtx);
void 		AEMtx33InvTranspose		(AEMtx33* pResult, AEMtx33* pMtx);

void 		AEMtx33Concat			(AEMtx33* pResult, AEMtx33* pMtx0, AEMtx33* pMtx1);

void 		AEMtx33Orthogonalize	(AEMtx33* pResult, AEMtx33* pMtx);

f32			AEMtx33Determinant		(AEMtx33* pMtx);

// ---------------------------------------------------------------------------

void 		AEMtx33SetCol			(AEMtx33* pResult, u32 col,  AEVec2* pVec);
void 		AEMtx33SetRow			(AEMtx33* pResult, u32 row,  AEVec2* pVec);
void 		AEMtx33GetCol			(AEVec2*  pResult, u32 col,  AEMtx33* pMtx);
void		AEMtx33GetRow			(AEVec2*  pResult, u32 row,  AEMtx33* pMtx);

// ---------------------------------------------------------------------------

void 		AEMtx33Trans			(AEMtx33* pResult, f32 x, f32 y);
void 		AEMtx33TransApply		(AEMtx33* pResult, AEMtx33* pMtx, f32 x, f32 y);

void 		AEMtx33Scale			(AEMtx33* pResult, f32 x, f32 y);
void 		AEMtx33ScaleApply		(AEMtx33* pResult, AEMtx33* pMtx, f32 x, f32 y);

void 		AEMtx33Rot				(AEMtx33* pResult, f32 angle);
void 		AEMtx33RotDeg			(AEMtx33* pResult, f32 angle);

// ---------------------------------------------------------------------------

void 		AEMtx33MultVec			(AEVec2* pResult, AEMtx33* pMtx, AEVec2* pVec);
void 		AEMtx33MultVecArray		(AEVec2* pResult, AEMtx33* pMtx, AEVec2* pVec, u32 count);
void 		AEMtx33MultVecSR		(AEVec2* pResult, AEMtx33* pMtx, AEVec2* pVec);
void 		AEMtx33MultVecArraySR	(AEVec2* pResult, AEMtx33* pMtx, AEVec2* pVec, u32 count);

// ---------------------------------------------------------------------------

#endif // MTX_H