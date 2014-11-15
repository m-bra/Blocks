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

#include "Types.hpp"
#include "EntityFuncs.hpp"
#include "BlockFuncs.hpp"

namespace physics
{

//template <typename BlockFieldArraySize, typename ChunkFieldArraySize>
template <typename Shared>
class Module
{
private:
	template <typename T>
	using ChunkFieldArray = ChunkFieldArray<typename Shared::ChunkFieldArraySize, T>;
	template <typename T>
	using BlockFieldArray = BlockFieldArray<typename Shared::BlockFieldArraySize, T>;

	Shared *shared;
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

	EntityFieldArray<fvec3> entityStartPos;
	EntityFieldArray<EntityPhysics> entityPhysics;

	btCollisionShape *playerShape;
	btMotionState *playerMotionState;
	btRigidBody *playerBody;
	fvec3 playerForce;
	float const playerHeight = 2;

	Module(Shared *shared);
	~Module();
	bool getSelectedBlock(fvec3::cref from, fvec3::cref to, ivec3 &b1, ivec3 &b2);
	int getSelectedEntity(fvec3::cref from, fvec3::cref to);
	void processDirtyEntity(int e);
	void update(Time time);
	void parallel(Time time);

	void move(ivec3_c &m)
	{
		auto createChunk = [&] (ivec3_c &c)
		{
			this->createChunk(c);
		};
		auto destroyChunk = [&] (ivec3_c &c)
		{
			this->destroyChunk(c);
		};
		chunkPhysics.shift(-m, createChunk, destroyChunk);
		chunkPhysics.iterate([&] (ivec3_c &c, ChunkPhysics &physics)
		{
			physics.body->getWorldTransform().setOrigin(c.bt() * Shared::CSIZE.bt());
			return true;
		});

		btTransform trans;

		playerBody->getWorldTransform().getOrigin()-= m.bt() * Shared::CSIZE.bt();
		entityPhysics.iterate([&] (int e, EntityPhysics &physics)
		{
			if (physics.created)
				physics.body->getWorldTransform().getOrigin()-= m.bt() * Shared::CSIZE.bt();
			return true;
		});
	}
};

template <typename Shared>
inline Module<Shared>::Module(Shared *shared) : shared(shared), blockFuncs(shared), entityFuncs(shared)
{
	broadphase = new btDbvtBroadphase();
	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	solver = new btSequentialImpulseConstraintSolver;
	physicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
	physicsWorld->setGravity(btVector3(0, -0, 0));

	chunkPhysics.iterate([&] (ivec3_c &c, ChunkPhysics &physics)
	{
		createChunk(c);
		return true;
	});

	playerShape = new btCapsuleShape(.2, playerHeight-.4);
	playerMotionState =
			new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(
				Shared::CCOUNT_X*Shared::CSIZE_X/2,
				Shared::CCOUNT_Y*Shared::CSIZE_Y+2,
				Shared::CCOUNT_Z*Shared::CSIZE_Z/2)));
	btScalar mass = 1;
	btVector3 inertia(0, 0, 0);
	playerShape->calculateLocalInertia(mass, inertia);
	playerBody = new btRigidBody(mass, playerMotionState, playerShape, inertia);

	physicsWorld->addRigidBody(playerBody);
}

template <typename Shared>
inline Module<Shared>::~Module()
{
	physicsWorld->removeRigidBody(playerBody);
	delete playerMotionState;
	delete playerBody;
	delete playerShape;

	chunkPhysics.iterate([&] (ivec3_c &c, ChunkPhysics &physics)
	{
		destroyChunk(c);
		return true;
	});

	delete physicsWorld;
	delete solver;
	delete collisionConfig;
	delete dispatcher;
	delete broadphase;
}

template <typename Shared>
inline bool Module<Shared>
::getSelectedBlock(fvec3_c &from, fvec3_c &to, ivec3 &b1, ivec3 &b2)
{
	btCollisionWorld::ClosestRayResultCallback rayCallBack(from.bt(), to.bt());
	physicsWorld->rayTest(from.bt(), to.bt(), rayCallBack);

	bool success = false;
	chunkPhysics.iterate([&] (ivec3_c &c, ChunkPhysics &physics)
	{
		if (rayCallBack.m_collisionObject == physics.body)
		{
			b1 = rayCallBack.m_hitPointWorld - rayCallBack.m_hitNormalWorld / 100;
			b2 = rayCallBack.m_hitPointWorld + rayCallBack.m_hitNormalWorld / 100;
			success = true;
		}
		return true;
	});

	return success;
}

template <typename Shared>
inline int Module<Shared>
::getSelectedEntity(fvec3_c &from, fvec3_c &to)
{
	btCollisionWorld::ClosestRayResultCallback rayCallBack(from.bt(), to.bt());
	physicsWorld->rayTest(from.bt(), to.bt(), rayCallBack);

	int result = -1;
	if (rayCallBack.hasHit())
	{
		entityPhysics.iterate([&] (int e, EntityPhysics &po)
		{
			if (rayCallBack.m_collisionObject == po.body)
				result = e;
			return true;
		});
	}

	return result;
}

template <typename Shared>
inline void Module<Shared>::createChunk(ivec3_c &c)
{
	ChunkPhysics &physics = chunkPhysics.chunkAt(c);
	physics.dirty = false;
	physics.shape = new btCompoundShape();
	physics.motionState = new btDefaultMotionState();
	physics.body = new btRigidBody(0, physics.motionState, physics.shape, btVector3(0, 0, 0));
	physicsWorld->addRigidBody(physics.body);
}

template <typename Shared>
inline void Module<Shared>::destroyChunk(ivec3_c &c)
{
	ChunkPhysics &physics = chunkPhysics.chunkAt(c);
	physicsWorld->removeRigidBody(physics.body);
	delete physics.motionState;
	delete physics.body;
	delete physics.shape;
}

template <typename Shared>
inline void Module<Shared>::processDirtyEntity(int e)
{
	EntityPhysics &physics = shared->physics.entityPhysics[e];
	EntityType &type = shared->entityTypes[e];

	assert(physics.dirty);
	physics.dirty = false;

	switch (type)
	{
	case EntityType::NONE:
		if (physics.created)
		{
			shared->physics.physicsWorld->removeRigidBody(physics.body);
			entityFuncs.onDestroy(e);
			assert(!physics.shape && !physics.motionState && !physics.body);
			physics.created = false;
		}
		break;
	default:
		if (!physics.created)
		{
			entityFuncs.onCreate(e);
			assert(physics.shape && physics.motionState && physics.body);
			physics.created = true;
			shared->physics.physicsWorld->addRigidBody(physics.body);
		}
		break;
	}
}

template <typename Shared>
inline void Module<Shared>::parallel(Time time)
{

}

template <typename Shared>
inline void Module<Shared>::update(Time time)
{
	// update chunks
	shared->physics.chunkPhysics.iterate([&] (ivec3_c &c, ChunkPhysics &physics)
	{
		if (physics.dirty)
		{
			Log::debug("physics::update: getting blockLock...");
			shared->blockWriteLock.lock(c);
			Log::debug("physics::update: got blockLock.");

			Log::debug("physics: update: getting move lock...");
			shared->moveLock.lock();
			Log::debug("physics: update: got move lock.");

			physics.dirty = false;
			delete physics.shape;
			physics.shape = new btCompoundShape(false);
			physics.body->setCollisionShape(physics.shape);
			physics.body->getWorldTransform().setOrigin(c.bt()
				* btVector3(Shared::CSIZE_X, Shared::CSIZE_Y, Shared::CSIZE_Z));

			shared->blockTypes.iterateInChunk(c, [&] (ivec3::cref b,
				BlockType const &type)
			{
				blockFuncs.addBlockShape(c, b, physics.shape);
				return true;
			});

			Log::debug("physics::update: unlocking moveLock...");
			shared->moveLock.unlock();
			Log::debug("physics::update: unlocked moveLock.");

			Log::debug("physics::update: unlocking blockLock...");
			shared->blockWriteLock.unlock(c);
			Log::debug("physics::update: unlocked blockLock.");
			return false;
		}
		return true;
	});

	// update entities
	shared->physics.entityPhysics.iterate([&] (int e, EntityPhysics &po)
	{
		if (po.dirty)
			processDirtyEntity(e);
		return true;
	});

	// update player
	playerBody->activate();
	playerBody->setAngularFactor(btVector3(0, 1, 0));
	playerBody->applyCentralForce(shared->physics.playerForce.bt());

	Log::debug("step:");
	physicsWorld->stepSimulation(time, 5);
	Log::debug("survived.");
}
}

#endif /* WORLDFIELDS_HPP_ */
