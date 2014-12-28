#ifndef WORLD_LISTENER_HPP_INCLUDED
#define WORLD_LISTENER_HPP_INCLUDED

#include "WorldTypes.hpp"

namespace blocks
{

class WorldListener
{
public:
    virtual void onWorldCreate(class World *world) = 0;
    virtual void onWorldDestroy() = 0;
    virtual void onWorldUpdate(Time time) = 0;
};

}

#endif//WORLD_LISTENER_HPP_INCLUDED
