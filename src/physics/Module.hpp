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

#include "../ChunkListener.hpp"
#include "../EntityListener.hpp"
#include "../LoadCallback.hpp"
#include "../WorldListener.hpp"

#include "Types.hpp"
#include "EntityFuncs.hpp"
#include "BlockFuncs.hpp"

namespace blocks
{

namespace physics
{

template <typename Shared>
class Module : public ChunkListener, public EntityListener, public LoadCallback, public WorldListener
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
	btCompoundShape *shapeBuf;
	std::mutex shapeBufLock;
	ivec3 shapeBufChunk;

	EntityFieldArray<EntityPhysics> entityPhysics;

	btCollisionShape *playerShape;
	btMotionState *playerMotionState;
	btRigidBody *playerBody;
	fvec3 playerForce;
	float const playerHeight = 2;

	void onWorldCreate(Shared *shared);
	void onWorldDestroy();

	void onEntityCreate(int e, std::initializer_list<void const*> args);
	void onEntityDestroy(int e);
	void onEntityUpdate(int e, Time time) {}
	void onEntityArrayResize(int newSize);

	void onWorldUpdate(Time time);

	bool doneLoading();

	bool getSelectedBlock(fvec3::cref from, fvec3::cref to, ivec3 &b1, ivec3 &b2);
	int getSelectedEntity(fvec3::cref from, fvec3::cref to);
	void processDirtyEntity(int e);
	void update(Time time);
	void parallel(Time time);

	bool canMove()
	{
		return !shapeBuf;
	}

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

	void onChunkChange(ivec3_c &c)
	{
		chunkPhysics.chunkAt(c).dirty = true;
	}
};

template <typename Shared>
inline void Module<Shared>::onWorldCreate(Shared *a_shared)
{
	shared = a_shared;
	blockFuncs.onWorldCreate(shared);
	entityFuncs.onWorldCreate(shared);

	broadphase = new btDbvtBroadphase();
	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	solver = new btSequentialImpulseConstraintSolver;
	physicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
	physicsWorld->setGravity(btVector3(0, -10, 0));

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
inline void Module<Shared>::onWorldDestroy()
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
inline void Module<Shared>::onEntityCreate(int e, std::initializer_list<void const*> args)
{
	EntityPhysics &physics = shared->physics.entityPhysics[e];

	assert (!physics.created);
	entityFuncs.onEntityCreate(e, args);
	assert(physics.shape && physics.motionState && physics.body);
	physics.created = true;
	shared->physics.physicsWorld->addRigidBody(physics.body);
}

template <typename Shared>
inline void Module<Shared>::onEntityDestroy(int e)
{
	EntityPhysics &physics = shared->physics.entityPhysics[e];

	assert(physics.created);
	shared->physics.physicsWorld->removeRigidBody(physics.body);
	entityFuncs.onEntityDestroy(e);
	assert(!physics.shape && !physics.motionState && !physics.body);
	physics.created = false;
}

template <typename Shared>
inline void Module<Shared>::onEntityArrayResize(int newSize)
{
	entityPhysics.resize(newSize);
}

template <typename Shared>
inline void Module<Shared>::onWorldUpdate(Time time)
{
	// flush shape buffer
	if (shapeBuf && shapeBufLock.try_lock())
	{
		ChunkPhysics &physics = chunkPhysics.chunkAt(shapeBufChunk);

		delete physics.shape;
		physics.shape = shapeBuf;
		shapeBuf = 0;

		physics.body->setCollisionShape(physics.shape);
		physics.body->getWorldTransform().setOrigin(shapeBufChunk.bt() * btVector3(Shared::CSIZE_X, Shared::CSIZE_Y, Shared::CSIZE_Z));
		shapeBufChunk = ivec3(-1, -1, -1);
		shapeBufLock.unlock();
	}

	// update player
	playerBody->activate();
	playerBody->setAngularFactor(btVector3(0, 1, 0));
	playerBody->applyCentralForce(shared->physics.playerForce.bt());

	//if (!shared->loading)
	physicsWorld->stepSimulation(time, 5);
}

template <typename Shared>
inline bool Module<Shared>::doneLoading()
{
	bool result = true;
	chunkPhysics.iterate([&] (ivec3_c &c, physics::ChunkPhysics &cp)
	{
		if (cp.dirty)
			result = false;
		return true;
	});
	return result;
}

template <typename Shared>
inline void Module<Shared>::parallel(Time time)
{
	// update chunks
	shared->physics.chunkPhysics.iterate([&] (ivec3_c &c, ChunkPhysics &physics)
	{
		if (physics.dirty && !shapeBuf)
		{
			shared->blockWriteLock.lock(c);
			shared->moveLock.lock();
			shapeBufLock.lock();

			shapeBufChunk = c;
			shapeBuf = new btCompoundShape(false);

			shared->blockTypes.iterateInChunk(c, [&] (ivec3::cref b, BlockType const &type)
			{
				blockFuncs.addBlockShape(c, b, shapeBuf);
				return true;
			});

			physics.dirty = false;

			shapeBufLock.unlock();
			shared->moveLock.unlock();
			shared->blockWriteLock.unlock(c);
			return false;
		}
		return true;
	});
}

}
}

#endif /* WORLDFIELDS_HPP_ */
