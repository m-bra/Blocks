#ifndef BASIC_LISTENER_HPP_INCLUDED
#define BASIC_LISTENER_HPP_INCLUDED

#include "SharedTypes.hpp"

namespace blocks
{

class WorldListener
{
public:
    virtual void onWorldCreate(class Shared *shared) = 0;
    virtual void onWorldDestroy() = 0;
    virtual void onWorldUpdate(Time time) = 0;
};

}

#endif
