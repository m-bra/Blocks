#include "precompiled.hpp"

#include "physics/EntityFuncs.hpp"
#include <cstring>

#include "physics/BulletPhysics.hpp"

namespace blocks
{

namespace physics
{

void EntityFuncs::onRegister()
{
    physics = dynamic_cast<BulletPhysics *>(parent);
}

void EntityFuncs::onEntityCreate(Entity e, EntityArgs args)
{
    assert(args.find("type") != args.end());

    EntityPhysics &po = physics->entityPhysics[e];
    EntityType type = (EntityType) args["type"];

    assert(!po.created);

    float mass = 1;
    btVector3 inertia;

    if (type == world->entityType.block)
    {
        po.shape = &blockShape;
        mass = .1;
    }
    else if (type == world->entityType.player)
    {
        mass = 50;
        po.shape = new btCapsuleShape(.2, world->playerHeight - .4);
    }
    else
        LOG_ERR("Physics: Trying to create not supported entity ", e);


    btVector3 pos = reinterpret_cast<fvec3_c *>(args["pos"])->bt();
    po.motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), pos));

    po.shape->calculateLocalInertia(mass, inertia);
    po.body = new btRigidBody(mass, po.motionState, po.shape, inertia);
    po.created = true;
	physics->physicsWorld->addRigidBody(po.body);
}

void EntityFuncs::onEntityDestroy(Entity e)
{
    EntityPhysics &po = physics->entityPhysics[e];
    assert(po.created);

	physics->physicsWorld->removeRigidBody(po.body);

    delete po.motionState;
    delete po.body;
    if (po.shape != &blockShape)
        delete po.shape;

    po.shape = 0;
    po.body = 0;
    po.motionState = 0;
    po.created = false;
}

void EntityFuncs::onUpdate(GameTime time)
{
    world->entityTypes.iterate([&](Entity e, EntityType const &type)
    {
        EntityPhysics &ephysics = physics->entityPhysics[e];

        if (type == world->entityType.player)
        {
            ephysics.body->activate();
            ephysics.body->setAngularFactor(btVector3(0, 1, 0));
        }
    });
}

}

}
