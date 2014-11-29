/*
 * Types.hpp
 *
 *  Created on: Oct 17, 2014
 *      Author: merlin
 */

#ifndef TYPES_HPP_
#define TYPES_HPP_


namespace physics
{

struct EntityPhysics
{
	bool dirty = false, created = false;
	btCollisionShape *shape = 0;
	btMotionState *motionState = 0;
	btRigidBody *body = 0;
};

struct ChunkPhysics
{
	bool dirty;
	btCompoundShape *shape;
	btMotionState *motionState;
	btRigidBody *body;
};

}


#endif /* TYPES_HPP_ */
