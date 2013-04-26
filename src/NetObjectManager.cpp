#include "NetObjectManager.h"
#include "Asteroids.h"

NetObjectManager::NetObjectManager(Asteroids * asgame)
	: NetInstContainer(), m_asGame(asgame)
{}

NetObjectManager::~NetObjectManager()
{}

NetInstContainer::iterator NetObjectManager::begin()
{
	return NetInstContainer::begin();
}

NetInstContainer::const_iterator NetObjectManager::begin() const
{
	return NetInstContainer::begin();
}

NetInstContainer::iterator NetObjectManager::end()
{
	return NetInstContainer::end();
}

NetInstContainer::const_iterator NetObjectManager::end() const
{
	return NetInstContainer::end();
}

void NetObjectManager::push(short netId, unsigned type, unsigned flag, float x, float y, float r)
{
	float scale = 1.0;
	AEVec2 pos; pos.x = x; pos.y = y;

	switch(type)
	{
	case TYPE_SHIP:
		type = TYPE_NET_SHIP;
		scale = SHIP_SIZE;
		break;
	};

	auto inst = m_asGame->gameObjInstCreate(type, scale, &pos, 0, r, true);
	emplace(std::make_pair(netId, inst));
}

void NetObjectManager::update(short netId, unsigned type, unsigned flag, float x, float y, float r)
{
	auto inst = find(netId);

	if( inst == end() )
	{
		push(netId, type, flag, x, y, r);
		return;
	}

	inst->second->flag = flag;
	inst->second->posCurr.x = x;
	inst->second->posCurr.y = y;
	inst->second->dirCurr = r;
}
