#ifndef PHYSICS_ENTITYFUNCS_HPP_INCLUDED
#define PHYSICS_ENTITYFUNCS_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#endif

#include "btBulletDynamicsCommon.h"
#include "../vec.hpp"

#include "Types.hpp"
#include "../Registerable.hpp"

namespace blocks
{

namespace physics {class Module;}

namespace physics
{
class EntityFuncs : public EntityListener, public WorldListener, public Registerable
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
