#ifndef PHYSICSWORLDFIELDS_HPP_
#define PHYSICSWORLDFIELDS_HPP_

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
	btCompoundShape *shapeBuf;
	std::mutex shapeBufLock;
	ivec3 shapeBufChunk;

	EntityFieldArray<EntityPhysics> entityPhysics;

	void onWorldCreate(World *world);
	void onWorldDestroy();
	void onWorldUpdate(Time time);
	WorldListener *getWorldListener() {return this;}

	void onEntityCreate(int e, EntityArgs args);
	void onEntityDestroy(int e);
	void onEntityUpdate(int e, Time time) {}
	void onEntityArrayResize(int newSize);
	EntityListener *getEntityListener() {return this;}

	bool doneLoading();
	LoadCallback *getLoadCallback() {return this;}

	ParallelCallback *getParallelCallback() {return this;}

	bool getSelectedBlock(fvec3_c &from, fvec3_c &to, ivec3 &b1, ivec3 &b2);
	int getSelectedEntity(fvec3_c &from, fvec3_c &to);
	void processDirtyEntity(int e);
	void update(Time time);
	void parallel(Time time);

	bool canMove() {return !shapeBuf;}
	void move(ivec3_c &m);
	void onChunkChange(ivec3_c &c) {chunkPhysics[c].dirty = true;}
	ChunkListener *getChunkListener() {return this;}

	void setEntityPos(int e, fvec3_c &pos)
	{
		btTransform t;
		entityPhysics[e].body->getMotionState()->getWorldTransform(t);
		t.setOrigin(pos.bt());
		entityPhysics[e].body->getMotionState()->setWorldTransform(t);
	}

	fvec3 getEntityPos(int e)
	{
		return entityPhysics[e].body->getWorldTransform().getOrigin();
	}

	void getEntityOpenGLMatrix(int e, float *matrix)
	{
		return entityPhysics[e].body->getWorldTransform().getOpenGLMatrix(matrix);
	}
};

}
}

#endif /* WORLDFIELDS_HPP_ */
