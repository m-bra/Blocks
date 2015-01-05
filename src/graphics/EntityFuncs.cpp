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

int uploadAACubeToVbo(float tx0, float tx1, float ty0, float ty1);

void EntityFuncs::onEntityCreate(int e, EntityArgs args)
{
    EntityType const &type = world->entityTypes[e];
    logic::EntityLogics &data = logic->entityLogics[e];
    EntityGraphics &eGraphics = graphics->entityGraphics[e];

    eGraphics.vertCount = 0;
    eGraphics.tbo = graphics->chunkTbo;
    glBindBuffer(GL_ARRAY_BUFFER, eGraphics.vbo);

    BlockType texType;
    switch (type)
    {
    case EntityType::PLAYER:
        break;
    case EntityType::BLOCK:
    {
        texType = data.blockEntity.blockType;
        auto t = blockFuncs->getBlockTypeTexCoords(texType);
        eGraphics.vertCount = uploadAACubeToVbo(t.first.x, t.first.x + t.second.x, t.first.y, t.first.y + t.second.y);
        break;
    }
    default:
        LOG_ERR("Cannot construct mesh for unknown entity type ", (int) type);
        break;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// call glBufferData on currently bound vbo with the vertices of an axis-aligned cube
int uploadAACubeToVbo(float tx0, float tx1, float ty0, float ty1)
{
    float const cube_verts[] = {
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
    glBufferData(GL_ARRAY_BUFFER, sizeof (cube_verts), cube_verts, GL_STATIC_DRAW);
    return sizeof (cube_verts) / sizeof (float);
}

}

}
