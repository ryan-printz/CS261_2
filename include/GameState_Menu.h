// ---------------------------------------------------------------------------
// Project Name		:	Asteroid Game
// File Name		:	GameState_Menu.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/02/05
// Purpose			:	header file for the 'menu' game state
// History			:
// - 2008/02/05		:	- initial implementation
// ---------------------------------------------------------------------------
#pragma once

#include "IGameState.h"
#include <string>
#include <list>

// ---------------------------------------------------------------------------

class GameState_Menu : public IGameState
{
public:
	GameState_Menu();
	virtual ~GameState_Menu();
	virtual void load(void);
	virtual void init(void);
	virtual void update(void);
	virtual void draw(void);
	virtual void free(void);
	virtual void unload(void);

private:
	int m_cursor;
	void * m_curl;

	struct tweet
	{
		tweet(const std::string & _user, const std::string &_text);

		std::string user;
		std::string text;
	};

	std::list<tweet> m_tweets;
};

// ---------------------------------------------------------------------------
