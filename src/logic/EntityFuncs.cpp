#include "EntityFuncs.hpp"

#include "Module.hpp"
#include "physics/Module.hpp"

namespace blocks
{

namespace logic
{

void EntityFuncs::onWorldCreate(World *a_world)
{
    world = a_world;
    physics = world->getFirstByType<physics::Module>();
}

void EntityFuncs::onEntityCreate(int e, EntityArgs ls)
{
	EntityType &type = world->entityTypes[e];
	EntityLogics &logics = logic->entityLogics[e];
	switch (type)
	{
	case EntityType::BLOCK:
		logics.blockEntity.fixTime = -1;
		break;
	case EntityType::PLAYER:
		logics.player.holdDistance = 3;
		break;
	default:;
	}
}

void EntityFuncs::onEntityUpdate(int e, Time time)
{
    EntityType &type = world->entityTypes[e];

    switch (type)
    {
    case EntityType::NONE:
        break;
    case EntityType::BLOCK:
    {
        EntityLogics::BlockEntity &data = logic->entityLogics[e].blockEntity;

        btRigidBody *body = physics->entityPhysics[e].body;
        btVector3 const pos = body->getWorldTransform().getOrigin();

        if (logic->entityLogics[world->playerEntity].player.heldEntity == e)
            data.target = physics->getEntityPos(shared->playerEntity) + world->camDir * logic->entityLogics[world->playerEntity].player.holdDistance;

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
                data.unactiveTime/= 2;
        }

        if (data.fixTime >= 0)
        {
            data.fixTime-= time;
            if (data.fixTime < 0)
            {
                world->setBlockType((ivec3) pos, logic.entityLogics[e].blockEntity.blockType);
                world->destroyEntity(e);
                world->graphics[0].buildChunk(ivec3(pos) / world->size);
            }
        }
    }
        break;
    case EntityType::BOT:
    {
        EntityLogics::Bot &data = logic->entityLogics[e].bot;
        btRigidBody *body = physics->entityPhysics[e].body;
        body->applyCentralForce(data.dir.bt());
        body->getWorldTransform().setRotation(btQuaternion(0, 0, 0, 1));

        fvec3 pos = body->getWorldTransform().getOrigin();

        bool const valid = world->blockTypes.isValidBlockCoord(pos);
        if (valid)
        {
            BlockType &type = world->blockTypes.blockAt(pos);
            if (type == BlockType::GROUND || type == BlockType::GROUND2)
            {
                fvec3 otherPos = pos - data.dir.normalized() * 2;
                int otherE = world->createEntity({{"type", (intptr_t) EntityType::BLOCK},
                    {"pos", (intptr_t) &otherPos}});
                EntityLogics::BlockEntity &otherData = logic->entityLogics[otherE].blockEntity;
                otherData.blockType = type;
                world->setBlockType(pos, BlockType::AIR);
            }
        }
    }
        break;
    default:
        std::cerr << "Trying to update entity (type = " << (int) type << ")\n";
    }
}


}

}
