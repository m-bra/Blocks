/*
 * Logic.hpp
 *
 *  Created on: Aug 30, 2014
 *      Author: merlin
 */

#ifndef LOGIC_HPP_
#define LOGIC_HPP_

#include <fstream>
#include <ctime>
#include <iostream>
#include <thread>
#include <chrono>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include <SFML/System.hpp>

#include <btBulletDynamicsCommon.h>

#include "../Logger.hpp"
#include "../BlockFieldArray.hpp"
#include "../ChunkFieldArray.hpp"
#include "../EntityFieldArray.hpp"
#include "../vec.hpp"

#include "../SharedTypes.hpp"
#include "Types.hpp"

#include "EntityFuncs.hpp"

namespace logic
{

template <typename Shared>
class Module
{
private:
	template <typename T>
	using ChunkFieldArray = ChunkFieldArray<typename Shared::ChunkFieldArraySize, T>;
	template <typename T>
	using BlockFieldArray = BlockFieldArray<typename Shared::BlockFieldArraySize, T>;

	Shared *shared;
	int seed;
	EntityFuncs<Shared> entityFuncs;
public:
	EntityFieldArray<EntityLogics> entityLogics;
	ChunkFieldArray<bool> chunkGenerateFlags;

	int heldEntity = -1;
	float holdDistance = 3;
	float reach = 50;

	Module(Shared *shared);
	~Module();

	void processDirtyEntity(int e);
	void generate(ivec3 const &c);
	void parallel(Time time);
	void update(Time time);
	void move(ivec3_c &m);

	void setWalk(fvec3_c &moveSpeeds);
	void jump();
	void resetPlayer();

	void use();
	void place();
	void take();
	void supertake();
};

template <typename Shared>
inline Module<Shared>::Module(Shared *shared) :
	shared(shared), entityFuncs(shared)
{
	shared->camDir = -fvec3::Y;
	shared->camLeft = -fvec3::X;
	shared->camUp = fvec3::Y;

	seed = time(0) % 10000;

	chunkGenerateFlags.fill(true);

	resetPlayer();
}

template <typename Shared>
inline Module<Shared>::~Module()
{

}

template <typename Shared>
inline void Module<Shared>::generate(ivec3_c &c)
{
	bool &genFlag = shared->logic.chunkGenerateFlags.chunkAt(c);
	assert (genFlag);
	genFlag = false;

	shared->blockWriteLock.lock(c);
	shared->moveLock.lock();
	shared->blockTypes.iterateInChunk(c, [&] (ivec3::cref b, BlockType &type)
	{
		ivec3 pos = b + shared->pos;

		float noise = glm::simplex(pos.glm() / 128.f) * .5;
		noise+= glm::simplex(pos.glm() / 64.f) * .5;
		noise = noise * .5 + .5;

		float treshold = ((float)pos.y / (Shared::CCOUNT_Y*Shared::CSIZE_Y));
		treshold = .3 + (treshold*treshold) * .5;
		type = (noise > treshold)
			? BlockType::GROUND
			: BlockType::AIR;

		return true;
	});
	for (int bx = 0; bx < Shared::CSIZE_X; ++bx)
	for (int bz = 0; bz < Shared::CSIZE_Z; ++bz)
	{
		for (int by = Shared::CSIZE_Y-1; by >= 0; --by)
		{
			BlockType &type = shared->blockTypes.blockInChunk(c, ivec3(bx,by,bz));
			if (type != BlockType::AIR)
			{
				type = BlockType::GROUND2;
				if (rand() % 100000 == 0)
					type = BlockType::COMPANION;
				break;
			}
		}
	}
	shared->moveLock.unlock();
	shared->blockWriteLock.unlock(c);

	shared->physics.chunkPhysics.chunkAt(c).dirty = true;
	shared->graphics.chunkGraphics.chunkAt(c).dirty = true;
}

/*
 * Movement is relative to the camera (including the direction)
 * e.g. vector (0,0,1) (pointing to z positive) is FORWARDS
 */
template <typename Shared>
inline void Module<Shared>::setWalk(fvec3_c &moveSpeeds)
{
	glm::vec3 removeY(1, 0, 1);
	shared->physics.playerForce =
		(shared->camDir * fvec3::XZ).normalize() * moveSpeeds.z * 15
		+(shared->camLeft * fvec3::XZ).normalize() * moveSpeeds.x * 15;
}

template <typename Shared>
inline void Module<Shared>::processDirtyEntity(int e)
{
	EntityLogics &logics = shared->logic.entityLogics[e];

	assert(logics.dirty);
	logics.dirty = false;

	switch (shared->entityTypes[e])
	{
	case EntityType::NONE:
		if (logics.created)
		{
			entityFuncs.onDestroy(e);
			logics.created = false;
		}
		break;
	default:
		if (!logics.created)
		{
			logics.created = true;
			entityFuncs.onCreate(e);
		}
		break;
	}
}

template <typename Shared>
inline void Module<Shared>::parallel(Time time)
{
	shared->logic.chunkGenerateFlags.iterate([&] (ivec3::cref c, bool &flag)
	{
		if (flag)
		{
			generate(c);
			return false;
		}
		return true;
	});
}

template <typename Shared>
inline void Module<Shared>::update(Time time)
{
	shared->gameTime+= time;

	// update entities
	shared->logic.entityLogics.iterate([&] (int e, EntityLogics &logics)
	{
		if (logics.dirty)
			processDirtyEntity(e);

		if (shared->isEntityCreated(e))
			entityFuncs.updateEntity(time, e);
		return true;
	});
}

template <typename Shared>
inline void Module<Shared>::move(ivec3_c &m)
{
	chunkGenerateFlags.shift(-m, [&] (ivec3 const &c)
	{
		chunkGenerateFlags.chunkAt(c) = true;
	}
	, [](ivec3_c &){});
}

template <typename Shared>
inline void Module<Shared>::resetPlayer()
{
	btVector3 &playerPos = shared->physics.playerBody->getWorldTransform().getOrigin();
	for (int i = 0; i < 50; ++i)
	{
		int bx = rand() % Shared::CCOUNT_X*Shared::CSIZE_X;
		int bz = rand() % Shared::CCOUNT_Z*Shared::CSIZE_Z;
		for (int by = Shared::CCOUNT_Y*Shared::CSIZE_Y-2; by >= 1; --by)
			if (shared->blockTypes.blockAt(ivec3(bx, by, bz)) == BlockType::AIR
			 && shared->blockTypes.blockAt(ivec3(bx, by+1, bz)) == BlockType::AIR
			 && shared->blockTypes.blockAt(ivec3(bx, by-1, bz)) == BlockType::GROUND2)
			{
				playerPos = btVector3(bx+.5, by+shared->physics.playerHeight/2, bz+.5);
				return;
			}
	}
}

template <typename Shared>
inline void Module<Shared>::jump()
{
	if (shared->onGround())
		shared->physics.playerBody->applyCentralImpulse(btVector3(0, 7, 0));
}

template <typename Shared>
inline void Module<Shared>::use()
{

}

template <typename Shared>
inline void Module<Shared>::take()
{
	// if selected block
	ivec3 b1, b2;
	bool const isSelectingBlock = shared->getSelectedBlock(b1, b2);
	bool const isBlockValid = shared->blockTypes.isValidBlockCoord(b1);

	int const selectedEntity = shared->getSelectedEntity();

	// push away held entity
	int &heldEntity = shared->logic.heldEntity;
	if (heldEntity != -1)
	{
		btRigidBody *body = shared->physics.entityPhysics[heldEntity].body;
		body->applyCentralImpulse(shared->camDir.bt() * 10 / body->getInvMass());
		entityFuncs.onDropEntity(heldEntity);
		heldEntity = -1;
	}
	else
	{
		if (isSelectingBlock && isBlockValid)
		{
			if (selectedEntity != -1)
				std::cerr << "Did select entity AND block simultaneously!\n";

			heldEntity = shared->createEntity(EntityType::BLOCK, b1 + fvec3(.5, .5, .5));

			shared->logic.entityLogics[heldEntity].blockEntity.blockType
				= shared->blockTypes.blockAt(b1);

			entityFuncs.onTakeEntity(heldEntity);

			shared->setBlockType(b1, BlockType::AIR);
		}
		else
		{
			heldEntity = selectedEntity;
			if (selectedEntity != -1)
				 entityFuncs.onTakeEntity(heldEntity);
		}
	}
}

template <typename Shared>
inline void Module<Shared>::supertake()
{
	int &heldEntity = shared->logic.heldEntity;
	if (heldEntity != -1)
	{
		btRigidBody *body = shared->physics.entityPhysics[heldEntity].body;
		glm::vec3 impulse = -glm::mix(shared->camDir.glm(), shared->camLeft.glm(), .3) * 30.f;
		body->applyCentralImpulse(fvec3(impulse).bt());
		entityFuncs.onDropEntity(heldEntity);
		heldEntity = -1;
	}
}

template <typename Shared>
inline void Module<Shared>::place()
{
	int &heldEnt = shared->logic.heldEntity;
	if (heldEnt != -1 && shared->entityTypes[heldEnt] == EntityType::BLOCK)
	{
		EntityLogics::BlockEntity &data = shared->logic.entityLogics[heldEnt].blockEntity;
		data.fixTime = 2;
		ivec3 pos = shared->physics.entityPhysics[heldEnt].body->getWorldTransform().getOrigin();
		data.target = pos + fvec3(.5, .5, .5);
		heldEnt = -1;
	}
}

}

#endif /* LOGIC_HPP_ */
