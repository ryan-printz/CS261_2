// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEInput.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	declaration for input stuff
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AE_INPUT_H
#define AE_INPUT_H

// ---------------------------------------------------------------------------
// Defines/Enums

// ---------------------------------------------------------------------------
// Externs

// ---------------------------------------------------------------------------
// class definitions

// ---------------------------------------------------------------------------
// Function prototypes

bool AEInputInit();
void AEInputReset();
void AEInputUpdate();
void AEInputExit();

u8 AEInputCheckCurr		(u8 key);
u8 AEInputCheckPrev		(u8 key);
u8 AEInputCheckTriggered(u8 key);
u8 AEInputCheckReleased	(u8 key);

// ---------------------------------------------------------------------------

#endif // AE_INPUT_H

