#pragma once

#include "IGameState.h"

class GameState_StartServer : public IGameState
{
public:
	GameState_StartServer();
	virtual ~GameState_StartServer() {};

	virtual void load(void);
	virtual void init(void);
	virtual void update(void);
	virtual void draw(void);
	virtual void free(void);
	virtual void unload(void);
};
