#include "precompiled.hpp"

#include "logic/EntityFuncs.hpp"

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

            btRigidBody *body = physics->entityPhysics[e].body;
            btVector3 const pos = body->getWorldTransform().getOrigin();

            if (e == world->entityHoldSlots[world->playerEntity][0])
                data.target = physics->getEntityPos(world->playerEntity) + world->graphics->camDir * world->entityHoldDistances[world->playerEntity][0];

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
                    world->setBlockType((ivec3) pos, logic->entityLogics[e].blockEntity.blockType);
                    world->killEntity(e);
                    world->graphics->buildChunk(ivec3(pos) / world->csize);
                }
            }
        }
    });
}

void EntityFuncs::onEntityDrop(Entity e, int slot, Entity holder)
{
    // assert type == BLOCK
    EntityLogics::BlockEntity &data = logic->entityLogics[e].blockEntity;
    data.moveToTarget = false;
}

void EntityFuncs::onEntityTake(Entity e, int slot, Entity holder)
{
	logic::EntityLogics::BlockEntity &data = logic->entityLogics[e].blockEntity;
	data.moveToTarget = true;
	data.fixTime = -1;
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
