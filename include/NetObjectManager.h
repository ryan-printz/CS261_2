#pragma once

#include <unordered_map>
#include "GameObject.h"

class Asteroids;

typedef std::unordered_map<short, GameObjInst*> NetInstContainer;

class NetObjectManager : protected NetInstContainer
{
public:
	NetObjectManager(Asteroids * asgame);
	~NetObjectManager();

	inline NetInstContainer::iterator begin();
	inline NetInstContainer::const_iterator begin() const;
	inline NetInstContainer::iterator end();
	inline NetInstContainer::const_iterator end() const;

	void push(short netId, unsigned type, unsigned flag, float x, float y, float r);
	void update(short netId, unsigned type, unsigned flag, float x, float y, float r);

protected:
	Asteroids * m_asGame;
};