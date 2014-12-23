/*
 * EntityFuncs.hpp
 *
 *  Created on: Oct 12, 2014
 *      Author: merlin
 */

#ifndef PENTITYFUNCS_HPP_
#define PENTITYFUNCS_HPP_

#include "btBulletDynamicsCommon.h"
#include "../vec.hpp"
#include "Types.hpp"
#include "../EntityListener.hpp"
#include "../WorldListener.hpp"
#include <cstring>
#include "../Logger.hpp"

namespace blocks
{

namespace physics
{
template <typename Shared>
class EntityFuncs : public EntityListener, public WorldListener
{
	Shared *shared;
	btBoxShape blockShape;
public:
	EntityFuncs() : blockShape(btVector3(.5, .5, .5)) {};

	void onWorldCreate(Shared *shared)
	{
		this->shared = shared;
	}

	void onWorldDestroy() {}
	void onWorldUpdate(Time time) {}

	void onEntityCreate(int e, EntityArgs args)
	{
		assert(args.find("type") != args.end());

		EntityPhysics &po = shared->physics.entityPhysics[e];
		EntityType type = (EntityType) args["type"];

		float mass = 1;
		btVector3 inertia;

		switch (type)
		{
		case EntityType::BLOCK:
			po.shape = &blockShape;
			mass = .1;
			break;
		case EntityType::PLAYER:
			mass = 50;
			po.shape = new btCapsuleShape(.2, shared->playerHeight - .4);
			break;
		default:
			Log::error("Physics: Trying to create not supported entity");
		}


		btVector3 pos = reinterpret_cast<fvec3_c *>(args["pos"])->bt();
		po.motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), pos));

		po.shape->calculateLocalInertia(mass, inertia);
		po.body = new btRigidBody(mass, po.motionState, po.shape, inertia);
	}

	void onEntityDestroy(int e)
	{
		EntityPhysics &po = shared->physics.entityPhysics[e];

		delete po.motionState;
		delete po.body;
		if (po.shape != &blockShape)
			delete po.shape;

		po.shape = 0;
		po.body = 0;
		po.motionState = 0;
	}

	void onEntityUpdate(int e, Time time)
	{
		EntityType type = shared->entityTypes[e];
		EntityPhysics physics = shared->physics.entityPhysics[e];

		physics.body->applyCentralForce(physics.force.bt());

		switch (type)
		{
		case EntityType::PLAYER:
			physics.body->activate();
			physics.body->setAngularFactor(btVector3(0, 1, 0));
			break;
		default:
			break;
		}
	}

	void onEntityArrayResize(int newsize) {}
};
}
}


#endif /* ENTITYFUNCS_HPP_ */
