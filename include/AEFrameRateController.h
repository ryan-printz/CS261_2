// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEFrameRateController.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2007/04/26
// Purpose			:	header file for the frame rate controller
// History			:
// - 2007/04/26		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AE_FRAME_RATE_CONTROLLER_H
#define AE_FRAME_RATE_CONTROLLER_H

// ---------------------------------------------------------------------------
// Externs

// frame controller related variables
extern f64	gAEFrameRateMax;	// clamp the frame rate to at most this number
extern f64	gAEFrameRate;		// the frame rate based on the last frame
extern f64	gAEFrameTime;		// time taken to process the last frame(in seconds)
extern u32	gAEFrameCounter;	// number of frame since the last reset

// ---------------------------------------------------------------------------
// Function prototypes

// Frame management
bool	AEFrameRateControllerInit	();
void	AEFrameRateControllerReset	();
void	AEFrameRateControllerStart	();
void	AEFrameRateControllerEnd	();

// ---------------------------------------------------------------------------

#endif // AE_FRAME_RATE_CONTROLLER_H

