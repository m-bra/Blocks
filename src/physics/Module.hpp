#ifndef PHYSICS_PHYSICS_HPP_INCLUDED
#define PHYSICS_PHYSICS_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#endif

#include <mutex>

#include <btBulletDynamicsCommon.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "../BlockFieldArray.hpp"
#include "../ChunkFieldArray.hpp"
#include "../EntityFieldArray.hpp"
#include "../vec.hpp"

#include "../Registerable.hpp"

#include "Types.hpp"
#include "EntityFuncs.hpp"
#include "BlockFuncs.hpp"

namespace blocks
{

namespace physics
{

class Module : public Registerable, public ChunkListener, public EntityListener, public LoadCallback, public WorldListener, public PhysicsCallback, public ParallelCallback
{
private:
	World *world;
	BlockFuncs blockFuncs;
	EntityFuncs entityFuncs;

	void createChunk(ivec3_c &c);
	void destroyChunk(ivec3_c &c);
public:
	btBroadphaseInterface *broadphase;
	btCollisionConfiguration *collisionConfig;
	btDispatcher *dispatcher;
	btSequentialImpulseConstraintSolver *solver;
	btDiscreteDynamicsWorld *physicsWorld;
	std::mutex physicsWorldBlock;

	ChunkFieldArray<ChunkPhysics> chunkPhysics;
	btCompoundShape *shapeBuf = 0;
	std::mutex shapeBufLock;
	ivec3 shapeBufChunk{0, 0, 0};

	EntityFieldArray<EntityPhysics> entityPhysics;

	void onWorldCreate(World *world);
	void onWorldDestroy();
	void onWorldUpdate(Time time);

	void onEntityArrayResize(int newSize)
	{
		entityPhysics.resize(newSize);
	}

	bool doneLoading();

	void getSubRegisterables(std::vector<Registerable *> &subs)
	{
		subs.push_back(&entityFuncs);
		subs.push_back(&blockFuncs);
	}

	bool getSelectedBlock(fvec3_c &from, fvec3_c &to, ivec3 &b1, ivec3 &b2);
	int getSelectedEntity(fvec3_c &from, fvec3_c &to);
	void processDirtyEntity(int e);
	void update(Time time);
	void parallel(Time time);

	bool canMove() {return !shapeBuf;}
	void move(ivec3_c &m);
	void onChunkChange(ivec3_c &c) {chunkPhysics[c].dirty = true;}

	void setEntityPos(int e, fvec3_c &pos)
	{
		entityPhysics[e].body->getWorldTransform().setOrigin(pos.bt());
	}

	fvec3 getEntityPos(int e)
	{
		return entityPhysics[e].body->getWorldTransform().getOrigin();
	}

	void getEntityOpenGLMatrix(int e, float *matrix)
	{
		return entityPhysics[e].body->getWorldTransform().getOpenGLMatrix(matrix);
	}

	void addEntityForce(int e, fvec3_c &f) {entityPhysics[e].force+= f;}
    void setEntityForce(int e, fvec3_c &f) {entityPhysics[e].force = f;}
};

}
}

#endif /* WORLDFIELDS_HPP_ */
