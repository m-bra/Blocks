#include "precompiled.hpp"

#include "BlockFuncs.hpp"

#include "btBulletDynamicsCommon.h"

#include "../Logger.hpp"

namespace blocks
{

namespace physics
{

void BlockFuncs::addBlockShape(ivec3_c &c, ivec3_c &b, btCompoundShape *chunkShape)
{
    static btBoxShape box(btVector3(.5, .5, .5));
    // b relative to its chunk
    ivec3 b_c = b - c * world->size;

    BlockType &type = world->blockTypes.blockAt(b);
    switch (type)
    {
    case BlockType::GROUND:
    case BlockType::GROUND2:
    case BlockType::COMPANION:
        chunkShape->addChildShape(btTransform(btQuaternion(0, 0, 0, 1), (b_c + .5).bt()), &box);
        break;
    case BlockType::AIR:
        break;
    default:
    {
        LOG_ERR("Trying to get shape of block which does not have one (blockType = ", (int) type, ")");
    }
    }
}

}

}
