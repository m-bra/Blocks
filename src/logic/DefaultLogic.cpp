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
    assert(genFlag);
    genFlag = false;

    world->chunkWriteLocks[c].lock();
    world->moveLock.lock();

    bool const cool_cave_world = false;

    if (cool_cave_world)
    {
        int const iters = 4;
        float const spawn_rate = 0.45;

        world->blockTypes.iterateInChunk(c, [&] (ivec3::cref b, BlockType &type)
        {
            float r = float(rand() % 1001) / 1000.0;
            type = r < spawn_rate ? world->blockType.ground : world->blockType.air;
            return true;
        });


        for (int i = 0; i < iters; ++i)
        {
            world->blockTypes.iterateInChunk(c, [&] (ivec3::cref b, BlockType &type)
            {
                float ground_count = 0;
                float air_count = 0;
                ivec3 neighbors[26] = {
                    // direct
                    b + ivec3::X, b - ivec3::X,
                    b + ivec3::Y, b - ivec3::Y,
                    b + ivec3::Z, b - ivec3::Z,
                    // corners
                    b + ivec3::X + ivec3::Y + ivec3::Z,
                    b - ivec3::X + ivec3::Y + ivec3::Z,
                    b + ivec3::X - ivec3::Y + ivec3::Z,
                    b - ivec3::X - ivec3::Y + ivec3::Z,
                    b + ivec3::X + ivec3::Y - ivec3::Z,
                    b - ivec3::X + ivec3::Y - ivec3::Z,
                    b + ivec3::X - ivec3::Y - ivec3::Z,
                    b - ivec3::X - ivec3::Y - ivec3::Z,
                    // intermediate stuff
                    b + ivec3::X + ivec3::Y,
                    b + ivec3::X - ivec3::Y,
                    b - ivec3::X + ivec3::Y,
                    b - ivec3::X - ivec3::Y,
                    b + ivec3::Z + ivec3::Y,
                    b + ivec3::Z - ivec3::Y,
                    b - ivec3::Z + ivec3::Y,
                    b - ivec3::Z - ivec3::Y,
                    b + ivec3::X + ivec3::Z,
                    b + ivec3::X - ivec3::Z,
                    b - ivec3::X + ivec3::Z,
                    b - ivec3::X - ivec3::Z,
                };

                for (int i = 0; i < 26; ++i)
                {
                    //LOG_DEBUG("neighbors[", i, "]=", neighbors[i]);
                    if (world->blockTypes.isValidBlockCoord(neighbors[i]))
                    {
                        BlockType type = world->blockTypes.blockAt(neighbors[i]);
                        if (type == world->blockType.air)
                            air_count+= 1;
                        else if (type == world->blockType.ground)
                            ground_count+= 1;
                        else
                        {
                            if (type != world->blockType.none)
                                LOG_ERR("Saw a block other than ground | air | none. (blockType=", (int)type, ")");
                            air_count+= 0.5;
                            ground_count+= 0.5;
                        }

                    }
                    else
                    {
                        //LOG_DEBUG("no valid block coord (max=", world->ccount * world->csize, ")");
                        air_count+= 0.5;
                        ground_count+= 0.5;
                    }
                }

                float density = ground_count / (ground_count + air_count);

                type = density < 0.35 ? world->blockType.air : type;
                type = density > 0.55 ? world->blockType.ground : type;
                //LOG_DEBUG("ground_count=", ground_count, ";air_count=", air_count, ";density=", density, ";type=", (int)type);

                return true;
            });
        }

    }
    else
    {
        world->blockTypes.iterateInChunk(c, [&] (ivec3::cref b, BlockType &type)
        {
            ivec3 pos = world->pos + b;

            float noise = glm::simplex(pos.glm() / 128.f) * .5;
            noise+= glm::simplex(pos.glm() / 64.f) * .5;
            noise = noise * .5 + .5;

            float treshold = ((float)pos.y / (world->ccount.y * world->csize.y));
            treshold = .0 + (treshold*treshold) * 1.5;

            if (noise > treshold)
                type = glm::simplex(pos.glm() / 32.f) * .25 + glm::simplex(pos.glm() / 256.f) * .75 > 0
                        ? world->blockType.ground : world->blockType.ground2;
            else
                type = world->blockType.air;
            return true;
        });
    }



    world->moveLock.unlock();
    world->chunkWriteLocks[c].unlock();

    world->onChunkChange(c, true);
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
        }
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
