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

template <typename Shared>
class Module : public Registerable, public ChunkListener, public EntityListener, public LoadCallback, public WorldListener, public PhysicsCallback
{
private:
	World *world;
	BlockFuncs<Shared> blockFuncs;
	EntityFuncs<Shared> entityFuncs;

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

	void THIS_IS_A_TEST__CAN_YOU_LEAVE_OUT_ONE_PARAM_NAME_BUT_HAVE_SOME_OTHERS(int, char const *n) {n = "yes!";}

	void onWorldCreate(Shared *shared);
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

	bool getSelectedBlock(fvec3_c &from, fvec3_c &to, ivec3 &b1, ivec3 &b2);
	int getSelectedEntity(fvec3_c &from, fvec3_c &to);
	void processDirtyEntity(int e);
	void update(Time time);
	void parallel(Time time);

	bool canMove() {return !shapeBuf;}
	void move(ivec3_c &m);
	void onChunkChange(ivec3_c &c) {chunkPhysics.chunkAt(c).dirty = true;}
	ChunkListener *getChunkListener() {return this;}

	void setEntityPos(int e, fvec3_c &pos)
	{
		btTransform t = entityPhysics[e].body->getMotionState()->getWorldTransform();
		t.setOrigin(pos.bt());
		entityPhysics[e].body->getMotionState()->setWorldTransform(t);
	}

	fvec3 getEntityPos(int e)
	{
		return entityPhysics[e].body->getMotionState()->getWorldTransform().getOrigin();
	}

	void getEntityOpenGLMatrix(int e, float *matrix)
	{
		return entityPhysics[e].body->getWorldTransform()->getOpenGLMatrix();	
	}
};

}
}

#endif /* WORLDFIELDS_HPP_ */
