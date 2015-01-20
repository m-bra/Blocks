#include "precompiled.hpp"

#include "physics/BlockFuncs.hpp"

#include "btBulletDynamicsCommon.h"

namespace blocks
{

namespace physics
{

void BlockFuncs::onRegister()
{
    setDoneLoading();
}

void BlockFuncs::addBlockShape(ivec3_c &c, ivec3_c &b, btCompoundShape *chunkShape)
{
    static btBoxShape box(btVector3(.5, .5, .5));
    // b relative to its chunk
    ivec3 b_c = b - c * world->csize;

    BlockType &type = world->blockTypes.blockAt(b);
    if (type == world->blockType.ground
        || type == world->blockType.ground2
        || type == world->blockType.companion)
    {
        chunkShape->addChildShape(btTransform(btQuaternion(0, 0, 0, 1), (b_c + .5).bt()), &box);
    }
}

}

}
