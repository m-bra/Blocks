#ifndef REGISTERABLE_HPP_INCLUDED
#define REGISTERABLE_HPP_INCLUDED

#include "LoadCallback.hpp"
#include "WorldListener.hpp"
#include "ChunkListener.hpp"
#include "EntityListener.hpp"
#include "GraphicsCallback.hpp"
#include "PhysicsCallback.hpp"
#include "ParallelCallback.hpp"

#include <vector>

namespace blocks
{

class Registerable
{
public:
    virtual void getSubRegisterables(std::vector<Registerable *> &) {}
    virtual void onRegister(class World *world) {}
};

}

#endif//REGISTERABLE_HPP_INCLUDED
