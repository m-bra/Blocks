#ifndef BASIC_LISTENER_HPP_INCLUDED
#define BASIC_LISTENER_HPP_INCLUDED

#include "SharedTypes.hpp"

namespace blocks
{

class WorldListener
{
public:
    virtual void onWorldCreate(class Shared *shared) {}
    virtual void onWorldDestroy() {}
    virtual void onWorldUpdate(Time time) {}
};

}

#endif
