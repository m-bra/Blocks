#ifndef REGISTERABLE_HPP_INCLUDED
#define REGISTERABLE_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#include "precompiled.hpp"
#endif

#include "World.hpp"

#include <vector>

namespace blocks
{

class Module
{
private:
    bool _loading = false;
    Module *_parent = 0;
    class World *_world = 0;
public:
    World *const &world = _world;
    Module *const &parent = _parent;
    bool const &loading = _loading;

    virtual ~Module() {}

    // modules pushed to vector will be added to the world.
    // each will be registered with this module as parent
    virtual void getSubModules(std::vector<Module *> &) {}

    // register this module with the world and an optional parent.
    // modules are always registered with one world at a time,
    // that means for every register() there is [must be] a deregister(),
    // none of register() and deregister() can be called consecutively
#   define register registeR
    void register(World *a_world, Module *a_parent = 0)
    {
        _world = a_world;
        _parent = a_parent;
        _loading = true;
        onRegister();
    }
    // see register()
    void deregister()
    {
        onDeregister();
        _world = 0;
        _parent = 0;
    }
    virtual void onRegister() {}
    virtual void onDeregister() {}
    // when all modules are loaded (i.e. loading = false), onStart() is called
    // the party can begin!!!
    virtual void onStart()    {    /*Well, not here :(*/   ;   }

    void update(float time) {onUpdate(time);}
    virtual void onUpdate(GameTime gtime) {}
    // this function is called from the parallel thread for heavy operations
    virtual void parallel(GameTime gtime) {}

    // set the loading flag to false
    // after registering all modules, world waits for them to set the loading flag to false
    void setDoneLoading() {_loading = false;}

    virtual void onEntityCreate(Entity e, std::vector<BaseEntityArgs *> const &args) {};
    virtual void onEntityDestroy(Entity e) {};
    virtual void onEntityCountChange(int newCount) {};
    virtual void onEntityDrop(Entity e, int slot, Entity holder) {}
    virtual void onEntityTake(Entity e, int slot, Entity holder) {}
    virtual void onEntityPlace(Entity e, int slot, Entity holder) {}

    virtual void onEntityTypeCreate(EntityType et) {}
    virtual void onBlockTypeCreate(BlockType bt) {}

    // "area" refers to the loaded area, i.e. the loaded chunks.
    // when moving the area, we shift the chunks in the memory

    // return whether this module is able to move now.
    // used to ensure that the parallel thread is synchronized.
    virtual bool canMoveArea() {return true;}
    virtual void moveArea(ivec3_c &m) {}

    // called when known that a chunk has changed (e.g. a block)
    virtual void onChunkChange(ivec3_c &c) {}
    // called when known that a block has changed
    // BUT not always when a block changes this method is called,
    // e.g./i.e when a whole chunk is generated, only onChunkChange() is called!
    virtual void onBlockChange(ivec3_c &b) {}
};

}

#endif//REGISTERABLE_HPP_INCLUDED
