#ifndef ENTITY_LISTENER_HPP_INCLUDED
#define ENTITY_LISTENER_HPP_INCLUDED

#include "SharedTypes.hpp"
#include <map>

namespace blocks
{

class EntityListener
{
public:
    virtual void onEntityCreate(int e, EntityArgs args) = 0;
    virtual void onEntityDestroy(int e) = 0;
    virtual void onEntityUpdate(int e, Time time) = 0;
    virtual void onEntityArrayResize(int newsize) = 0;
};

}
#endif/*ENTITY_LISTENER_HPP_INCLUDED*/
