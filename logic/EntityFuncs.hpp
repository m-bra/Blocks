/*
 * EntityFuncs.hpp
 *
 *  Created on: Oct 13, 2014
 *      Author: merlin
 */

#ifndef LENTITYFUNCS_HPP_
#define LENTITYFUNCS_HPP_

#include "../vec.hpp"
#include "../SharedTypes.hpp"
#include "Types.hpp"

namespace logic
{

template <typename Shared>
class EntityFuncs
{
private:
	Shared *shared;
public:
	EntityFuncs(Shared *shared) : shared(shared) {};

	void onCreate(int e)
	{
		EntityType &type = shared->entityTypes[e];
		EntityLogics &logics = shared->logic.entityLogics[e];
		switch (type)
		{
		case EntityType::BLOCK:
			logics.blockEntity.fixTime = -1;
			break;
		default:;
		}
	}

	void onDestroy(int e)
	{

	}

	void updateEntity(Time time, int e)
	{
		EntityType &type = shared->entityTypes[e];

		switch (type)
		{
		case EntityType::BLOCK:
		{
			EntityLogics::BlockEntity &data = shared->logic.entityLogics[e].blockEntity;

			btRigidBody *body = shared->physics.entityPhysics[e].body;
			btVector3 const pos = body->getWorldTransform().getOrigin();

			if (shared->logic.heldEntity == e)
				data.target = shared->camPos + shared->camDir * shared->logic.holdDistance;

			if (data.moveToTarget)
			{
				// position
				body->setLinearVelocity((data.target.bt() - pos) * 5);

				// angle
				body->getWorldTransform().setRotation(btQuaternion(0, 0, 0, 1));
				body->setAngularVelocity(btVector3(0, 0, 0));

				body->activate();
			}
			else
			{
				if (!body->isActive())
				{
					data.unactiveTime+= time;
					if (data.unactiveTime > 60)
					{
						data.target = (ivec3) pos;
						data.target+= .5;
						data.moveToTarget = true;
						data.fixTime = 2;
					}
				}
				else
					data.unactiveTime = 0;
			}

			if (data.fixTime >= 0)
			{
				data.fixTime-= time;
				if (data.fixTime <= 0)
				{
					shared->setBlockType((ivec3) pos, shared->logic.entityLogics[e].blockEntity.blockType);
					shared->destroyEntity(e);
					shared->graphics.buildChunk(ivec3(pos) / Shared::CSIZE);
				}
			}
		}
			break;
		case EntityType::BOT:
		{
			EntityLogics::Bot &data = shared->logic.entityLogics[e].bot;
			btRigidBody *body = shared->physics.entityPhysics[e].body;
			body->applyCentralForce(data.dir.bt());
			body->getWorldTransform().setRotation(btQuaternion(0, 0, 0, 1));

			fvec3 pos = body->getWorldTransform().getOrigin();

			bool const valid = shared->blockTypes.isValidBlockCoord(pos);
			if (valid)
			{
				BlockType &type = shared->blockTypes.blockAt(pos);
				if (type == BlockType::GROUND || type == BlockType::GROUND2)
				{
					int otherE = shared->createEntity(EntityType::BLOCK,
						pos - data.dir.normalized() * 2);
					EntityLogics::BlockEntity &otherData = shared->logic.entityLogics[otherE].blockEntity;
					otherData.blockType = type;
					shared->setBlockType(pos, BlockType::AIR);
				}
			}
		}
			break;
		default:
			std::cerr << "Trying to update entity (type = " << (int) type << ")\n";
		}
	}

	void onDropEntity(int e)
	{
		// assert type == BLOCK
		EntityLogics::BlockEntity &data = shared->logic.entityLogics[e].blockEntity;
		data.moveToTarget = false;
	}

	void onTakeEntity(int e)
	{
		logic::EntityLogics::BlockEntity &data = shared->logic.entityLogics[e].blockEntity;
		data.moveToTarget = true;
		data.fixTime = -1;
	}
};

}  // namespace logic



#endif /* ENTITYFUNCS_HPP_ */
