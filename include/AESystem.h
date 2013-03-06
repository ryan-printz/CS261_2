// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AESystem.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	header file for the system module
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AE_SYSTEM_H
#define AE_SYSTEM_H

// ---------------------------------------------------------------------------

// window related variables
extern HINSTANCE	ghAESysAppInstance;
extern HWND			ghAESysWin;
extern WNDCLASS		gAESysWinClass;
extern bool			gAESysWinExists;

extern const char*	gpAESysWinTitle;
extern const char*	gpAESysWinClassName;

extern bool			gAESysAppActive;

// ---------------------------------------------------------------------------

bool AESysInit	(HINSTANCE hAppInstance, int show);
void AESysReset	();
void AESysUpdate();
void AESysExit	();

// ---------------------------------------------------------------------------

void AESysFrameStart();
void AESysFrameEnd();

// ---------------------------------------------------------------------------

#endif // AE_SYSTEM_H

