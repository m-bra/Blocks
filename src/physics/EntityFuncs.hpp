#ifndef PENTITYFUNCS_HPP_INCLUDED
#define PENTITYFUNCS_HPP_INCLUDED

#include "btBulletDynamicsCommon.h"
#include "../vec.hpp"
#include <cstring>

#include "Types.hpp"
#include "../Registerable.hpp"
#include "../Logger.hpp"

namespace blocks
{

namespace physics {class Module;}

namespace physics
{
class EntityFuncs : public EntityListener, public WorldListener
{
	World *world;
	physics::Module *physics;
	btBoxShape blockShape;
public:
	EntityFuncs() : blockShape(btVector3(.5, .5, .5)) {};

	void onWorldCreate(World *world);
	void onWorldDestroy() {}
	void onWorldUpdate(Time time) {}

	void onEntityCreate(int e, EntityArgs args);
	void onEntityDestroy(int e);
	void onEntityUpdate(int e, Time time);
	void onEntityArrayResize(int newsize) {}
};
}
}


#endif /* ENTITYFUNCS_HPP_ */
