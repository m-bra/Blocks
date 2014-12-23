#ifndef TYPES_HPP_
#define TYPES_HPP_

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
