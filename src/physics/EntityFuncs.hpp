#ifndef PHYSICS_ENTITYFUNCS_HPP_INCLUDED
#define PHYSICS_ENTITYFUNCS_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#include "precompiled.hpp"
#endif

#include "btBulletDynamicsCommon.h"

#include "Module.hpp"


namespace blocks
{

namespace physics
{
class EntityFuncs : public Module
{
	World *world;
	class BulletPhysics *physics;
	btBoxShape blockShape;
public:
	EntityFuncs() : blockShape(btVector3(.5, .5, .5)) {};

	void onRegister() override;
	void onUpdate(GameTime time)override;

	void onEntityCreate(Entity e, EntityArgs args) override;
	void onEntityDestroy(Entity e) override;
};
}
}

#endif/*PHYSICS_ENTITYFUNCS_HPP_INCLUDED*/
