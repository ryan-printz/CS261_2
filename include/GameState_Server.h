#pragma once

#include "GameState_BasePlay.h"

class GameState_Server : public GameState_BasePlay
{
public:
	GameState_Server();
	virtual ~GameState_Server();

	virtual void update();
	virtual void draw();

	virtual void onEnd() {};
};