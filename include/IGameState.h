#pragma once

#include "GameStateMgr.h"

class IGameState 
{
public:
	IGameState()
		: m_gsm(nullptr), m_state(nullptr), m_lastState(nullptr)
	{}

	virtual ~IGameState()
	{};

	virtual void load(void) = 0;
	virtual void init(void) = 0;
	virtual void update(void) = 0;
	virtual void draw(void) = 0;
	virtual void free(void) = 0;
	virtual void unload(void) = 0;

	void setGameStateManager(GameStateManager * gsm) { m_gsm = gsm; }

	struct State
	{
		State(IGameState * parent) : m_parent(parent) {};

		virtual void update() = 0;
		virtual void draw() = 0;

	protected:
		void nextGameState(IGameState * gameState) { m_parent->m_gsm->nextState(gameState); }
		void popGameState() { m_parent->m_gsm->popState(); }

		IGameState * m_parent;
	};
	friend struct State;

	void nextState(State * state) { delete m_lastState; m_lastState = m_state; m_state = state; }
protected:
	GameStateManager * m_gsm;
	State * m_state, * m_lastState;
};
