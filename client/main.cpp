// ---------------------------------------------------------------------------
// Project Name		:	Omega Library Test Program
// File Name		:	main.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2007/04/26
// Purpose			:	main entry point for the test program
// History			:
// - 2007/04/26		:	- initial implementation
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
#include "main.h"
#include "ISocket.h"
#include "GameState_Menu.h"

extern int gWIN_WIDTH;
extern int gWIN_HEIGHT;

int WINAPI WinMain(HINSTANCE instanceH, HINSTANCE prevInstanceH, LPSTR command_line, int show)
{
	gWIN_WIDTH = 800;
	gWIN_HEIGHT = 600;
	// Initialize the system
	AESysInit (instanceH, show);
	initSockets(true);

	GameStateManager gsm;

	gsm.init(new GameState_Menu());

	while(!gsm.quit())
	{
		// reset the system modules
		AESysReset();

		// If not restarting, load the gamestate
		if(!gsm.restart())
			gsm.loadState();
		else
			gsm.restartState();

		// Initialize the gamestate
		gsm.initState();

		while(!gsm.changeState())
		{
			AESysFrameStart();

			AEInputUpdate();

			gsm.updateState();
			gsm.drawState();
			
			AESysFrameEnd();

			// check if forcing the application to quit
			gsm.quit((gAESysWinExists == false) || AEInputCheckTriggered(DIK_ESCAPE));
		}
		
		gsm.freeState();

		if(!gsm.restart())
			gsm.unloadState();

		gsm.nextState();
	}

	// free the system
	cleanSockets();
	AESysExit();
}