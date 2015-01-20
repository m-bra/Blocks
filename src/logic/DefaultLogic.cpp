#include "precompiled.hpp"

#include "logic/DefaultLogic.hpp"

#include <fstream>
#include <ctime>
#include <iostream>
#include <thread>
#include <chrono>

#include <glm/gtc/noise.hpp>

#include "physics/BulletPhysics.hpp"
#include "GraphicsProvider.hpp"

namespace blocks
{

namespace logic
{

void DefaultLogic::onRegister()
{
    setDoneLoading();

    physics = world->getFirstModuleByType<physics::BulletPhysics>();
    chunkGenerateFlags.create(world->ccount);

    chunkGenerateFlags.fill(true);
}

void DefaultLogic::onDeregister()
{
    chunkGenerateFlags.destroy();
}

void DefaultLogic::generate(ivec3_c &c)
{
    bool &genFlag = chunkGenerateFlags[c];
    assert (genFlag);
    genFlag = false;

    world->chunkWriteLocks[c].lock();
    world->moveLock.lock();
    world->blockTypes.iterateInChunk(c, [&] (ivec3::cref b, BlockType &type)
    {
        ivec3 pos = world->pos + b;

        float noise = glm::simplex(pos.glm() / 128.f) * .5;
        noise+= glm::simplex(pos.glm() / 64.f) * .5;
        noise = noise * .5 + .5;

        float treshold = ((float)pos.y / (world->ccount.y * world->csize.y));
        treshold = .3 + (treshold*treshold) * .5;
        type = (noise > treshold)
        ? world->blockType.ground
        : world->blockType.air;

        return true;
    });
    for (int bx = 0; bx < world->csize.x; ++bx)
        for (int bz = 0; bz < world->csize.z; ++bz)
        {
            for (int by = world->csize.y-1; by >= 0; --by)
            {
                BlockType &type = world->blockTypes.blockInChunk(c, ivec3(bx,by,bz));
                if (type != world->blockType.air)
                {
                    type = world->blockType.ground2;
                    if (rand() % 100000 == 0)
                        type = world->blockType.companion;
                    break;
                }
            }
        }
    world->moveLock.unlock();
    world->chunkWriteLocks[c].unlock();

    world->onChunkChange(c);
}

void DefaultLogic::onEntityCreate(Entity e, std::vector<BaseEntityArgs *> const &args)
{
    EntityLogics &logics = entityLogics[e];

    assert(!logics.created);
    logics.created = true;
}


void DefaultLogic::onEntityDestroy(Entity e)
{
    EntityLogics &logics = entityLogics[e];

    assert(logics.created);
    logics.created = false;
}

void DefaultLogic::parallel(GameTime time)
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

void DefaultLogic::onUpdate(GameTime time)
{
    world->gameTime+= time;
    if (loading)
    {
        bool allGenerated = true;
        chunkGenerateFlags.iterate([&] (ivec3_c &c, bool &flag)
        {
            if (flag)
                allGenerated = false;
            return true;
        });
        if (allGenerated)
            setDoneLoading();
    }
}


void DefaultLogic::moveArea(ivec3_c &m)
{
    chunkGenerateFlags.shift(-m, [&] (ivec3 const &c)
    {
        chunkGenerateFlags[c] = true;
    }
    , [](ivec3_c &){});
}

}

}
