#ifndef REGISTERABLE_HPP_INCLUDED
#define REGISTERABLE_HPP_INCLUDED

#include "LoadCallback.hpp"
#include "WorldListener.hpp"
#include "ChunkListener.hpp"
#include "EntityListener.hpp"
#include "GraphicsCallback.hpp"
#include "PhysicsCallback.hpp"
#include "ParallelCallback.hpp"

namespace blocks
{

class Registerable
{
public:
    virtual EntityListener *getEntityListener() {return 0;}
    virtual LoadCallback *getLoadCallback() {return 0;}
    virtual ChunkListener *getChunkListener() {return 0;}
    virtual WorldListener *getWorldListener() {return 0;}
    virtual ParallelCallback *getParallelCallback() {return 0;}
    virtual void onRegister(class World *world) {}
};

}

#endif//REGISTERABLE_HPP_INCLUDED
