#include "precompiled.hpp"

#include "EntityFuncs.hpp"
#include "Module.hpp"
#include "logic/Module.hpp"
#include "BlockFuncs.hpp"

namespace blocks
{

namespace graphics
{

void EntityFuncs::onRegister(World *world)
{
    logic = world->getFirstRegisterableByType<logic::Module>();
    assert(logic);
    graphics = world->getFirstRegisterableByType<Module>();
    assert(graphics);
    blockFuncs = world->getFirstRegisterableByType<BlockFuncs>();
    assert(blockFuncs);
}

void EntityFuncs::onEntityCreate(int e, EntityArgs args)
{
    EntityType const &type = world->entityTypes[e];
    logic::EntityLogics &data = logic->entityLogics[e];
    EntityGraphics &eGraphics = graphics->entityGraphics[e];

    eGraphics.tbo = graphics->chunkTbo;

    BlockType texType;
    switch (type)
    {
    case EntityType::BLOCK:
        texType = data.blockEntity.blockType;
        break;
    case EntityType::BOT:
        texType = BlockType::COMPANION;
        break;
    default:
        std::stringstream ss;
        ss << "Trying to get vertices of entity which does not have any (type = " << (int) type << ")";
        Log::error(ss);
        break;
    }
    auto t = blockFuncs->getBlockTypeTexCoords(texType);

    float tx0 = t.first.x;
    float tx1 = t.first.x + t.second.x;
    float ty0 = t.first.y;
    float ty1 = t.first.y + t.second.y;
    float cube_verts[] = {
        // left
        -.5, -.5, -.5,  tx0, ty0,  -1, 0, 0,
        -.5, +.5, +.5,  tx1, ty1,  -1, 0, 0,
        -.5, +.5, -.5,  tx1, ty0,  -1, 0, 0,
        -.5, -.5, -.5,  tx0, ty0,  -1, 0, 0,
        -.5, -.5, +.5,  tx0, ty1,  -1, 0, 0,
        -.5, +.5, +.5,  tx1, ty1,  -1, 0, 0,

        // right
        +.5, -.5, -.5,  tx0, ty0,  +1, 0, 0,
        +.5, +.5, -.5,  tx1, ty0,  +1, 0, 0,
        +.5, +.5, +.5,  tx1, ty1,  +1, 0, 0,
        +.5, -.5, -.5,  tx0, ty0,  +1, 0, 0,
        +.5, +.5, +.5,  tx1, ty1,  +1, 0, 0,
        +.5, -.5, +.5,  tx0, ty1,  +1, 0, 0,

        // bottom
        -.5, -.5, -.5,  tx0, ty0,  0, -1, 0,
        +.5, -.5, -.5,  tx1, ty0,  0, -1, 0,
        +.5, -.5, +.5,  tx1, ty1,  0, -1, 0,
        -.5, -.5, -.5,  tx0, ty0,  0, -1, 0,
        +.5, -.5, +.5,  tx1, ty1,  0, -1, 0,
        -.5, -.5, +.5,  tx0, ty1,  0, -1, 0,

        // top
        -.5, +.5, -.5,  tx0, ty0,  0, +1, 0,
        +.5, +.5, +.5,  tx1, ty1,  0, +1, 0,
        +.5, +.5, -.5,  tx1, ty0,  0, +1, 0,
        -.5, +.5, -.5,  tx0, ty0,  0, +1, 0,
        -.5, +.5, +.5,  tx0, ty1,  0, +1, 0,
        +.5, +.5, +.5,  tx1, ty1,  0, +1, 0,

        // back
        -.5, -.5, -.5,  tx0, ty0,  0, 0, -1,
        +.5, +.5, -.5,  tx1, ty1,  0, 0, -1,
        +.5, -.5, -.5,  tx1, ty0,  0, 0, -1,
        -.5, -.5, -.5,  tx0, ty0,  0, 0, -1,
        -.5, +.5, -.5,  tx0, ty1,  0, 0, -1,
        +.5, +.5, -.5,  tx1, ty1,  0, 0, -1,

        // front
        -.5, -.5, +.5,  tx0, ty0,  0, 0, +1,
        +.5, -.5, +.5,  tx1, ty0,  0, 0, +1,
        +.5, +.5, +.5,  tx1, ty1,  0, 0, +1,
        -.5, -.5, +.5,  tx0, ty0,  0, 0, +1,
        +.5, +.5, +.5,  tx1, ty1,  0, 0, +1,
        -.5, +.5, +.5,  tx0, ty1,  0, 0, +1,
    };
    glBindBuffer(GL_ARRAY_BUFFER, eGraphics.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof (cube_verts), cube_verts, GL_STATIC_DRAW);
    eGraphics.vertCount = sizeof (cube_verts) / sizeof (float);
}

}

}
