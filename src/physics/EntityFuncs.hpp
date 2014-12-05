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

	void onEntityCreate(int e, std::initializer_list<void const*> args)
	{
		EntityPhysics &po = shared->physics.entityPhysics[e];

		po.shape = &blockShape;

		float mass = .1;
		btVector3 inertia;
		po.shape->calculateLocalInertia(mass, inertia);

		btVector3 pos;
		for (auto it = args.begin(); it != args.end(); it+= 2)
			if (strcmp(reinterpret_cast<char const*>(*it), "pos") == 0)
				pos = reinterpret_cast<fvec3 const *>(*(it + 1))->bt();
		po.motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), pos));

		po.body = new btRigidBody(mass, po.motionState, po.shape, inertia);
	}

	void onEntityDestroy(int e)
	{
		EntityPhysics &po = shared->physics.entityPhysics[e];
		delete po.motionState;
		delete po.body;

		po.shape = 0;
		po.body = 0;
		po.motionState = 0;
	}
};
}
}


#endif /* ENTITYFUNCS_HPP_ */
