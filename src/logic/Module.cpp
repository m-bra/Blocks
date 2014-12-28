#include "Module.hpp"

#include "../physics/Module.hpp"

namespace blocks
{

namespace logic
{

void Module::onWorldCreate(World *a_world)
{
    world = a_world;
    chunkGenerateFlags.create(world->count);

    world->camDir = -fvec3::Y;
    world->camLeft = -fvec3::X;
    world->camUp = fvec3::Y;

    chunkGenerateFlags.fill(true);

    entityFuncs.onWorldCreate(world);

    physics = world->getFirstRegisterableByType<physics::Module>();
}

void Module::onWorldDestroy()
{
    chunkGenerateFlags.destroy();
}

void Module::generate(ivec3_c &c)
{
    bool &genFlag = chunkGenerateFlags[c];
    assert (genFlag);
    genFlag = false;

    world->blockWriteLock.lock(c);
    world->moveLock.lock();
    world->blockTypes.iterateInChunk(c, [&] (ivec3::cref b, BlockType &type)
    {
        ivec3 pos = world->pos + b;

        float noise = glm::simplex(pos.glm() / 128.f) * .5;
        noise+= glm::simplex(pos.glm() / 64.f) * .5;
        noise = noise * .5 + .5;

        float treshold = ((float)pos.y / (world->count.y * world->size.y));
        treshold = .3 + (treshold*treshold) * .5;
        type = (noise > treshold)
        ? BlockType::GROUND
        : BlockType::AIR;

        return true;
    });
    for (int bx = 0; bx < world->size.x; ++bx)
        for (int bz = 0; bz < world->size.z; ++bz)
        {
            for (int by = world->size.y-1; by >= 0; --by)
            {
                BlockType &type = world->blockTypes.blockInChunk(c, ivec3(bx,by,bz));
                if (type != BlockType::AIR)
                {
                    type = BlockType::GROUND2;
                    if (rand() % 100000 == 0)
                        type = BlockType::COMPANION;
                    break;
                }
            }
        }
    world->moveLock.unlock();
    world->blockWriteLock.unlock(c);

    world->onChunkChange(c);
}

/*
* Movement is relative to the camera (including the direction)
* e.g. vector (0,0,1) (pointing to z positive) is FORWARDS
*/
void Module::setWalk(fvec3_c &moveSpeeds)
{
    glm::vec3 removeY(1, 0, 1);
    physics->entityPhysics[world->playerEntity].force =
        (world->camDir * fvec3::XZ).normalize() * moveSpeeds.z * 15
       +(world->camLeft * fvec3::XZ).normalize() * moveSpeeds.x * 15;
}


void Module::onEntityCreate(int e, EntityArgs args)
{
    EntityLogics &logics = entityLogics[e];

    assert(!logics.created);
    logics.created = true;
    entityFuncs.onEntityCreate(e, args);
}


void Module::onEntityDestroy(int e)
{
    EntityLogics &logics = entityLogics[e];

    assert(logics.created);
    logics.created = false;
    entityFuncs.onEntityDestroy(e);
}


void Module::onEntityUpdate(int e, Time time)
{
    entityFuncs.onEntityUpdate(e, time);
}


bool Module::doneLoading()
{
    bool result = true;
    chunkGenerateFlags.iterate([&] (ivec3_c &c, bool &flag)
    {
        if (flag)
            result = false;
        return true;
    });
    return result;
}


void Module::parallel(Time time)
{
    chunkGenerateFlags.iterate([&] (ivec3::cref c, bool &flag)
    {
        if (flag)
        {
            generate(c);
            return false;
        }
        return true;
    });
}


void Module::onWorldUpdate(Time time)
{
    world->gameTime+= time;
}


void Module::move(ivec3_c &m)
{
    chunkGenerateFlags.shift(-m, [&] (ivec3 const &c)
    {
        chunkGenerateFlags[c] = true;
    }
    , [](ivec3_c &){});
}


void Module::resetPlayer()
{
    fvec3 playerPos = world->getEntityPos(world->playerEntity);
    int bx = world->count.x * world->size.x / 2;
    int bz = world->count.z * world->size.z / 2;
    for (int i = 0; i < 50; ++i)
    {
        for (int by = world->count.y * world->size.y - 2; by >= 1; --by)
            if (world->blockTypes.blockAt(ivec3(bx, by, bz)) == BlockType::AIR
                && world->blockTypes.blockAt(ivec3(bx, by+1, bz)) == BlockType::AIR
                && world->blockTypes.blockAt(ivec3(bx, by-1, bz)) == BlockType::GROUND2)
            {
                world->setEntityPos(world->playerEntity, btVector3(bx+.5, by+world->playerHeight/2, bz+.5));
                return;
            }
        bx = rand() % world->count.x * world->size.x;
        bz = rand() % world->size.z * world->size.z;
    }
    std::cerr << "ERROR: Cannot reset player.\n";
}

void Module::jump()
{
    // sometimes doesnt jump although on ground
    //if (world->onGround())
    physics->entityPhysics[world->playerEntity].body->applyCentralImpulse(btVector3(0, 7, 0));
}


void Module::use()
{

}


void Module::take()
{
    // if selected block
    ivec3 b1, b2;
    fvec3 from, to;
    from = physics->getEntityPos(world->playerEntity);
    to = from + world->camDir * 10;
    bool const isSelectingBlock = physics->getSelectedBlock(from, to, b1, b2);
    bool const isBlockValid = world->blockTypes.isValidBlockCoord(b1);

    int const selectedEntity = physics->getSelectedEntity(from, to);

    // push away held entity
    int &heldEntity = entityLogics[world->playerEntity].player.heldEntity;
    if (heldEntity != -1)
    {
        btRigidBody *body = physics->entityPhysics[heldEntity].body;
        body->applyCentralImpulse(world->camDir.bt() * 10 / body->getInvMass());
        entityFuncs.onEntityDrop(heldEntity);
        heldEntity = -1;
    }
    else
    {
        if (isSelectingBlock && isBlockValid)
        {
            if (selectedEntity != -1)
                std::cerr << "Did select entity AND block simultaneously!\n";

            fvec3 pos = b1 + fvec3(.5, .5, .5);
            heldEntity = world->createEntity({{"type", (intptr_t) EntityType::BLOCK}, {"pos", (intptr_t)&pos}});

            entityLogics[heldEntity].blockEntity.blockType
            = world->blockTypes.blockAt(b1);

            entityFuncs.onEntityTake(heldEntity);

            world->setBlockType(b1, BlockType::AIR);
        }
        else
        {
            heldEntity = selectedEntity;
            if (selectedEntity != -1)
                entityFuncs.onEntityTake(heldEntity);
        }
    }
}


void Module::supertake()
{
    int &heldEntity = entityLogics[world->playerEntity].player.heldEntity;
    if (heldEntity != -1)
    {
        btRigidBody *body = physics->entityPhysics[heldEntity].body;
        glm::vec3 impulse = -glm::mix(world->camDir.glm(), world->camLeft.glm(), .3) * 30.f;
        body->applyCentralImpulse(fvec3(impulse).bt());
        entityFuncs.onEntityDrop(heldEntity);
        heldEntity = -1;
    }
}


void Module::place()
{
    int &heldEnt = entityLogics[world->playerEntity].player.heldEntity;
    if (heldEnt != -1 && world->entityTypes[heldEnt] == EntityType::BLOCK)
    {
        EntityLogics::BlockEntity &data = entityLogics[heldEnt].blockEntity;
        data.fixTime = 2;
        ivec3 pos = physics->entityPhysics[heldEnt].body->getWorldTransform().getOrigin();
        data.target = pos + fvec3(.5, .5, .5);
        heldEnt = -1;
    }
}

}

}
