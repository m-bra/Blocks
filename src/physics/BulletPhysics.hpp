#ifndef PHYSICS_PHYSICS_HPP_INCLUDED
#define PHYSICS_PHYSICS_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#include "precompiled.hpp"
#endif

#include <mutex>

#include <btBulletDynamicsCommon.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "Module.hpp"
#include "PhysicsProvider.hpp"

#include "physics/Types.hpp"
#include "physics/EntityFuncs.hpp"
#include "physics/BlockFuncs.hpp"


namespace blocks
{

namespace physics
{

class BulletPhysics : public Module, public PhysicsProvider
{
private:
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

	void onRegister() override;
	void onDeregister() override;
	void onUpdate(GameTime time) override;

	void onEntityCountChange(int newSize) override
	{
		entityPhysics.resize(newSize);
	}

	void getSubModules(std::vector<Module *> &subs) override
	{
		subs.push_back(&entityFuncs);
		subs.push_back(&blockFuncs);
	}

	bool getSelectedBlock(fvec3_c &from, fvec3_c &to, ivec3 &b1, ivec3 &b2) override;
	int getSelectedEntity(fvec3_c &from, fvec3_c &to) override;
	void processDirtyEntity(Entity e);
	void parallel(GameTime time) override;

	bool canMoveArea()  override {return !shapeBuf;}
	void moveArea(ivec3_c &m) override;
	void onChunkChange(ivec3_c &c) override {chunkPhysics[c].dirty = true;}

	void setEntityPos(Entity e, fvec3_c &pos) override
	{
		assert(entityPhysics[e].created);
		entityPhysics[e].body->getWorldTransform().setOrigin(pos.bt());
	}

	fvec3 getEntityPos(Entity e) override
	{
		assert(entityPhysics[e].created);
		return entityPhysics[e].body->getWorldTransform().getOrigin();
	}

	void getEntityOpenGLMatrix(Entity e, float *matrix) override
	{
		assert(entityPhysics[e].created);
		return entityPhysics[e].body->getWorldTransform().getOpenGLMatrix(matrix);
	}

	void addEntityForce(Entity e, fvec3_c &f) override
	{
		assert(entityPhysics[e].created);
		entityPhysics[e].body->applyCentralForce(f.bt());
	}

    void setEntityForce(Entity e, fvec3_c &f) override
	{
		assert(entityPhysics[e].created);
		entityPhysics[e].body->clearForces();
		addEntityForce(e, f);
	}

	void addEntityImpulse(Entity e, fvec3_c &f) override
	{
		assert(entityPhysics[e].created);
		entityPhysics[e].body->applyCentralImpulse(f.bt());
	}

    void setEntityImpulse(Entity e, fvec3_c &f) override
	{
		assert(entityPhysics[e].created);
		entityPhysics[e].body->setLinearVelocity(btVector3(0, 0, 0));
		addEntityImpulse(e, f);
	}

	void addEntityVelocity(Entity e, fvec3_c &f) override
	{
		assert(entityPhysics[e].created);
		entityPhysics[e].body->setLinearVelocity(entityPhysics[e].body->getLinearVelocity() + f.bt());
	}

    void setEntityVelocity(Entity e, fvec3_c &f) override
	{
		assert(entityPhysics[e].created);
		entityPhysics[e].body->setLinearVelocity(f.bt());
	}
};

}
}

#endif /* WORLDFIELDS_HPP_ */
