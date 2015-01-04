#ifndef WORLD_LISTENER_HPP_INCLUDED
#define WORLD_LISTENER_HPP_INCLUDED

#include "WorldTypes.hpp"

namespace blocks
{

class WorldListener
{
public:
    virtual void onWorldCreate(class World *world) {}
    virtual void onWorldDestroy() {}
    virtual void onWorldUpdate(Time time) {}
};

}

#endif//WORLD_LISTENER_HPP_INCLUDED
