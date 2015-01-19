#include "precompiled.hpp"

#include "physics/BulletPhysics.hpp"

namespace blocks
{

namespace physics
{

void BulletPhysics::moveArea(ivec3_c &m)
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
		physics.body->getWorldTransform().setOrigin(c.bt() * world->csize.bt());
		return true;
	});

	btTransform trans;

	entityPhysics.iterate([&] (Entity e, EntityPhysics &physics)
	{
		if (physics.created)
			physics.body->getWorldTransform().getOrigin()-= m.bt() * world->csize.bt();
		return true;
	});
}

void BulletPhysics::onRegister()
{
	chunkPhysics.create(world->ccount);

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
}

void BulletPhysics::onDeregister()
{
	chunkPhysics.iterate([&] (ivec3_c &c, ChunkPhysics &physics)
	{
		destroyChunk(c);
		return true;
	});

	chunkPhysics.destroy();

	delete physicsWorld;
	delete solver;
	delete collisionConfig;
	delete dispatcher;
	delete broadphase;
}

bool BulletPhysics::getSelectedBlock(fvec3_c &from, fvec3_c &to, ivec3 &b1, ivec3 &b2)
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

int BulletPhysics::getSelectedEntity(fvec3_c &from, fvec3_c &to)
{
	btCollisionWorld::ClosestRayResultCallback rayCallBack(from.bt(), to.bt());
	physicsWorld->rayTest(from.bt(), to.bt(), rayCallBack);

	int result = -1;
	if (rayCallBack.hasHit())
	{
		entityPhysics.iterate([&] (Entity e, EntityPhysics &po)
		{
			if (rayCallBack.m_collisionObject == po.body)
				result = e;
			return true;
		});
	}

	return result;
}

void BulletPhysics::createChunk(ivec3_c &c)
{
	ChunkPhysics &physics = chunkPhysics[c];
	physics.dirty = false;
	physics.shape = new btCompoundShape();
	physics.motionState = new btDefaultMotionState();
	physics.body = new btRigidBody(0, physics.motionState, physics.shape, btVector3(0, 0, 0));
	physicsWorld->addRigidBody(physics.body);
}

void BulletPhysics::destroyChunk(ivec3_c &c)
{
	ChunkPhysics &physics = chunkPhysics[c];
	physicsWorld->removeRigidBody(physics.body);
	delete physics.motionState;
	delete physics.body;
	delete physics.shape;
}

void BulletPhysics::onUpdate(GameTime time)
{
	// flush shape buffer
	if (shapeBuf && shapeBufLock.try_lock())
	{
		ChunkPhysics &physics = chunkPhysics[shapeBufChunk];

		delete physics.shape;
		physics.shape = shapeBuf;
		shapeBuf = 0;

		physics.body->setCollisionShape(physics.shape);
		physics.body->getWorldTransform().setOrigin(shapeBufChunk.bt() * world->csize.bt());
		shapeBufChunk = ivec3(-1, -1, -1);
		shapeBufLock.unlock();
	}

	bool noDirty = true;
	chunkPhysics.iterate([&] (ivec3_c &c, physics::ChunkPhysics &cp)
	{
		if (cp.dirty)
			noDirty = false;
		return true;
	});
	if (noDirty)
		setDoneLoading();

	//if (!world->loading)
	physicsWorld->stepSimulation(time, 5);
}

void BulletPhysics::parallel(GameTime time)
{
	// update chunks
	chunkPhysics.iterate([&] (ivec3_c &c, ChunkPhysics &physics)
	{
		if (physics.dirty && !shapeBuf)
		{
			world->chunkWriteLocks[c].lock();
			world->moveLock.lock();
			shapeBufLock.lock();

			shapeBufChunk = c;
			shapeBuf = new btCompoundShape(false);

			world->blockTypes.iterateInChunk(c, [&] (ivec3::cref b, BlockType const &type)
			{
				blockFuncs.addBlockShape(c, b, shapeBuf);
				return true;
			});

			physics.dirty = false;

			shapeBufLock.unlock();
			world->moveLock.unlock();
			world->chunkWriteLocks[c].unlock();
			return false;
		}
		return true;
	});
}

}

}
