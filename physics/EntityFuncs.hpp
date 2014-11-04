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

namespace physics
{
template <typename Shared>
class EntityFuncs
{
	Shared *shared;
	btBoxShape blockShape;
public:
	EntityFuncs(Shared *shared) : shared(shared), blockShape(btVector3(.5, .5, .5)) {};

	void onCreate(int e)
	{
		EntityPhysics &po = shared->physics.entityPhysics[e];

		po.shape = &blockShape;

		float mass = .1;
		btVector3 inertia;
		po.shape->calculateLocalInertia(mass, inertia);

		btVector3 pos = shared->physics.entityStartPos[e].bt();
		po.motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), pos));

		po.body = new btRigidBody(mass, po.motionState, po.shape, inertia);
	}

	void onDestroy(int e)
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



#endif /* ENTITYFUNCS_HPP_ */
