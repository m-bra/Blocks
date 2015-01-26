#include "precompiled.hpp"

#include "graphics/EntityFuncs.hpp"

#include "graphics/DefaultGraphics.hpp"
#include "graphics/BlockFuncs.hpp"
#include "logic/DefaultLogic.hpp"

namespace blocks
{

namespace graphics
{

void EntityFuncs::onRegister()
{
    setDoneLoading();

    logic = world->getFirstModuleByType<logic::DefaultLogic>();
    assert(logic);
    graphics = dynamic_cast<DefaultGraphics *>(parent);
    assert(graphics);
    blockFuncs = world->getFirstModuleByType<BlockFuncs>();
    assert(blockFuncs);
}

int uploadAACubeToVbo(float tx0, float tx1, float ty0, float ty1);

void EntityFuncs::onEntityCreate(Entity e, std::vector<BaseEntityArgs *> const &args)
{
    EntityType const &type = world->entityTypes[e];
    ::blocks::logic::EntityLogics &data = logic->entityLogics[e];
    EntityGraphics &eGraphics = graphics->entityGraphics[e];

    eGraphics.vertCount = 0;
    eGraphics.tbo = graphics->chunkTbo;
    glBindBuffer(GL_ARRAY_BUFFER, eGraphics.vbo);

    BlockType texType;
    if (type == world->entityType.block)
    {
        texType = data.blockEntity.blockType;
        auto t = blockFuncs->getBlockTypeTexCoords(texType);
        eGraphics.vertCount = uploadAACubeToVbo(t.first.x, t.first.x + t.second.x, t.first.y, t.first.y + t.second.y);
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
