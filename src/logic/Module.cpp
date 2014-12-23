#include "Module.hpp"

namespace blocks
{

namespace logic
{

void Module::onWorldCreate(Shared *a_shared)
{
    shared = a_shared;
    shared->camDir = -fvec3::Y;
    shared->camLeft = -fvec3::X;
    shared->camUp = fvec3::Y;

    chunkGenerateFlags.fill(true);

    entityFuncs.onWorldCreate(shared);
}


void Module::generate(ivec3_c &c)
{
    bool &genFlag = shared->logic.chunkGenerateFlags.chunkAt(c);
    assert (genFlag);
    genFlag = false;

    shared->blockWriteLock.lock(c);
    shared->moveLock.lock();
    shared->blockTypes.iterateInChunk(c, [&] (ivec3::cref b, BlockType &type)
    {
        ivec3 pos = shared->pos + b;

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

                    void Module::setWalk(fvec3_c &moveSpeeds)
                    {
                        glm::vec3 removeY(1, 0, 1);
                        shared->physics.entityPhysics[shared->playerEntity].force =
                        (shared->camDir * fvec3::XZ).normalize() * moveSpeeds.z * 15
                        +(shared->camLeft * fvec3::XZ).normalize() * moveSpeeds.x * 15;
                    }


                    void Module::onEntityCreate(int e, EntityArgs args)
                    {
                        EntityLogics &logics = shared->logic.entityLogics[e];

                        assert(!logics.created);
                        logics.created = true;
                        entityFuncs.onEntityCreate(e, args);
                    }


                    void Module::onEntityDestroy(int e)
                    {
                        EntityLogics &logics = shared->logic.entityLogics[e];

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


                            void Module::onWorldUpdate(Time time)
                            {
                                shared->gameTime+= time;
                            }


                            void Module::move(ivec3_c &m)
                            {
                                chunkGenerateFlags.shift(-m, [&] (ivec3 const &c)
                                {
                                    chunkGenerateFlags.chunkAt(c) = true;
                                }
                                , [](ivec3_c &){});
                            }


                            void Module::resetPlayer()
                            {
                                btVector3 &playerPos = shared->entityPos[shared->playerEntity];
                                int bx = Shared::CCOUNT_X*Shared::CSIZE_X / 2;
                                int bz = Shared::CCOUNT_Z*Shared::CSIZE_Z / 2;
                                for (int i = 0; i < 50; ++i)
                                    {
                                        for (int by = Shared::CCOUNT_Y*Shared::CSIZE_Y-2; by >= 1; --by)
                                            if (shared->blockTypes.blockAt(ivec3(bx, by, bz)) == BlockType::AIR
                                                && shared->blockTypes.blockAt(ivec3(bx, by+1, bz)) == BlockType::AIR
                                                && shared->blockTypes.blockAt(ivec3(bx, by-1, bz)) == BlockType::GROUND2)
                                                {
                                                    playerPos = btVector3(bx+.5, by+shared->playerHeight/2, bz+.5);
                                                    return;
                                                }
                                                bx = rand() % Shared::CCOUNT_X*Shared::CSIZE_X;
                                                bz = rand() % Shared::CCOUNT_Z*Shared::CSIZE_Z;
                                            }
                                            std::cerr << "ERROR: Cannot reset player.\n";
                                        }


                                        void Module::jump()
                                        {
                                            // sometimes doesnt jump although on ground
                                            //if (shared->onGround())
                                            shared->physics.entityPhysics[shared->playerEntity].body->applyCentralImpulse(btVector3(0, 7, 0));
                                        }


                                        void Module::use()
                                        {

                                        }


                                        void Module::take()
                                        {
                                            // if selected block
                                            ivec3 b1, b2;
                                            bool const isSelectingBlock = shared->getSelectedBlock(b1, b2);
                                            bool const isBlockValid = shared->blockTypes.isValidBlockCoord(b1);

                                            int const selectedEntity = shared->getSelectedEntity();

                                            // push away held entity
                                            int &heldEntity = shared->logic.entityLogics[shared->playerEntity].player.heldEntity;
                                            if (heldEntity != -1)
                                                {
                                                    btRigidBody *body = shared->physics.entityPhysics[heldEntity].body;
                                                    body->applyCentralImpulse(shared->camDir.bt() * 10 / body->getInvMass());
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
                                                                    heldEntity = shared->createEntity({{"type", (intptr_t) EntityType::BLOCK}, {"pos", (intptr_t)&pos}});

                                                                    shared->logic.entityLogics[heldEntity].blockEntity.blockType
                                                                    = shared->blockTypes.blockAt(b1);

                                                                    entityFuncs.onEntityTake(heldEntity);

                                                                    shared->setBlockType(b1, BlockType::AIR);
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
                                                                    int &heldEntity = shared->logic.entityLogics[shared->playerEntity].player.heldEntity;
                                                                    if (heldEntity != -1)
                                                                        {
                                                                            btRigidBody *body = shared->physics.entityPhysics[heldEntity].body;
                                                                            glm::vec3 impulse = -glm::mix(shared->camDir.glm(), shared->camLeft.glm(), .3) * 30.f;
                                                                            body->applyCentralImpulse(fvec3(impulse).bt());
                                                                            entityFuncs.onEntityDrop(heldEntity);
                                                                            heldEntity = -1;
                                                                        }
                                                                    }


                                                                    void Module::place()
                                                                    {
                                                                        int &heldEnt = shared->logic.entityLogics[shared->playerEntity].player.heldEntity;
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

}
