#ifndef PHYSICS_TYPES_HPP_INCLUDED
#define PHYSICS_TYPES_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#endif

#include "../vec.hpp"

class btCollisionShape;
class btMotionState;
class btCompoundShape;
class btRigidBody;

namespace blocks
{

namespace physics
{

struct EntityPhysics
{
	bool created = false;
	btCollisionShape *shape = 0;
	btMotionState *motionState = 0;
	btRigidBody *body = 0;
	fvec3 force;
};

struct ChunkPhysics
{
	bool dirty;
	btCompoundShape *shape;
	btMotionState *motionState;
	btRigidBody *body;
};

}

}


#endif /* TYPES_HPP_ */
