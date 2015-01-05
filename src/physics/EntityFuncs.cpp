#include "precompiled.hpp"

#include "EntityFuncs.hpp"
#include <cstring>

#include "Module.hpp"

namespace blocks
{

namespace physics
{

void EntityFuncs::onRegister(World *world)
{
    physics = world->getFirstRegisterableByType<physics::Module>();
}

void EntityFuncs::onWorldCreate(World *world)
{
    this->world = world;
}

void EntityFuncs::onEntityCreate(int e, EntityArgs args)
{
    assert(args.find("type") != args.end());

    EntityPhysics &po = physics->entityPhysics[e];
    EntityType type = (EntityType) args["type"];

    assert(!po.created);

    float mass = 1;
    btVector3 inertia;

    switch (type)
    {
    case EntityType::BLOCK:
        po.shape = &blockShape;
        mass = .1;
        break;
    case EntityType::PLAYER:
        mass = 50;
        po.shape = new btCapsuleShape(.2, world->playerHeight - .4);
        break;
    default:
        LOG_ERR("Physics: Trying to create not supported entity ", e);
    }


    btVector3 pos = reinterpret_cast<fvec3_c *>(args["pos"])->bt();
    po.motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), pos));

    po.shape->calculateLocalInertia(mass, inertia);
    po.body = new btRigidBody(mass, po.motionState, po.shape, inertia);
    po.created = true;
	physics->physicsWorld->addRigidBody(po.body);
}

void EntityFuncs::onEntityDestroy(int e)
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

void EntityFuncs::onEntityUpdate(int e, Time time)
{
    EntityType const type = world->entityTypes[e];
    EntityPhysics &ephysics = physics->entityPhysics[e];

    ephysics.body->applyCentralForce(ephysics.force.bt());

    switch (type)
    {
    case EntityType::PLAYER:
        ephysics.body->activate();
        ephysics.body->setAngularFactor(btVector3(0, 1, 0));
        break;
    default:
        break;
    }
}

}

}
