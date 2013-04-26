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

	NetInstContainer::iterator begin();
	NetInstContainer::const_iterator begin() const;
	NetInstContainer::iterator end();
	NetInstContainer::const_iterator end() const;

	void push(short netId, unsigned type, unsigned flag, float x, float y, float r, float velx, float vely);
	void update(short netId, unsigned type, unsigned flag, float x, float y, float r, float velx, float vely);

protected:
	Asteroids * m_asGame;
};