#ifndef ENTITY_LISTENER_HPP_INCLUDED
#define ENTITY_LISTENER_HPP_INCLUDED

#include "WorldTypes.hpp"
#include <map>

namespace blocks
{

class EntityListener
{
public:
    virtual void onEntityCreate(int e, EntityArgs args) {};
    virtual void onEntityDestroy(int e) {};
    virtual void onEntityUpdate(int e, Time time) {};
    virtual void onEntityArrayResize(int newsize) {};
};

}
#endif/*ENTITY_LISTENER_HPP_INCLUDED*/
