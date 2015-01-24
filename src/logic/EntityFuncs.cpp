#include "precompiled.hpp"

#include "logic/EntityFuncs.hpp"

#include "PhysicsProvider.hpp"
#include "GraphicsProvider.hpp"

#include "logic/DefaultLogic.hpp"
#include "physics/BulletPhysics.hpp"

namespace blocks
{

namespace logic
{

void EntityFuncs::onRegister()
{
    setDoneLoading();

    physics = world->getFirstModuleByType<physics::BulletPhysics>();
    assert(physics);
    logic = dynamic_cast<DefaultLogic *>(parent);
    assert(logic);
}

void EntityFuncs::onEntityCreate(Entity e, std::vector<BaseEntityArgs *> const &args)
{
	EntityType &type = world->entityTypes[e];
	EntityLogics &logics = logic->entityLogics[e];
    if (type == world->entityType.block)
    {
        logics.blockEntity.blockType = getFirstByType<DefaultLogic::EntityArgs>(args)->blockEntityBlockType;
        logics.blockEntity.fixTime = -1;
    }
    else if (type == world->entityType.player)
    {
        world->entityHoldDistances[e][0] = 3;
    }
}

void EntityFuncs::onUpdate(GameTime time)
{
    world->entityTypes.iterate([&](int e, EntityType &type)
    {
        if (type == world->entityType.block)
        {
            EntityLogics::BlockEntity &data = logic->entityLogics[e].blockEntity;

            fvec3 const pos = world->getEntityPos(e);

            if (e == world->entityHoldSlots[world->playerEntity][0])
                data.target = world->getEntityPos(world->playerEntity) + world->entityEyePos[world->playerEntity] + world->graphics->camDir * world->entityHoldDistances[world->playerEntity][0];

            btRigidBody *body = physics->entityPhysics[e].body;

            if (data.moveToTarget)
            {
                // move
                fvec3 dis = data.target - pos;
                if (dis.length2() > 1./(1000 * 1000))
                {
                    if (dis.length2() < 1./(128 * 128))
                        dis = dis.normalize() * 1./(128 * 128);
                    world->physics->setEntityVelocity(e, dis * 5);
                }
                else
                    world->physics->setEntityVelocity(e, fvec3(0, 0, 0));

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
                    if (world->blockTypes.isValidBlockCoord((ivec3) pos))
                    {
                        world->setBlockType((ivec3) pos, logic->entityLogics[e].blockEntity.blockType);
                        world->graphics->buildChunk(ivec3(pos) / world->csize);
                    }
                    world->killEntity(e);
                }
            }
        }
    });
}

void EntityFuncs::onEntityDrop(Entity e, int slot, Entity holder)
{
    EntityType const &type = world->entityTypes[e];
    if (type == world->entityType.block)
    {
        EntityLogics::BlockEntity &data = logic->entityLogics[e].blockEntity;
        data.moveToTarget = false;
    }
}

void EntityFuncs::onEntityTake(Entity e, int slot, Entity holder)
{
    if (e != -1)
    {
        EntityType const &type = world->entityTypes[e];
        if (type == world->entityType.block)
        {
        	logic::EntityLogics::BlockEntity &data = logic->entityLogics[e].blockEntity;
        	data.moveToTarget = true;
        	data.fixTime = -1;
        }
    }
}

void EntityFuncs::onEntityPlace(Entity e, int slot, Entity holder)
{
    if (e != -1)
    {
		if (world->entityTypes[e] == world->entityType.block)
		{
	        EntityLogics::BlockEntity &data = logic->entityLogics[e].blockEntity;
	        data.fixTime = 2;
	        ivec3 pos = world->getEntityPos(e);
	        data.target = pos + fvec3(.5, .5, .5);
		}
    }
}

}

}
