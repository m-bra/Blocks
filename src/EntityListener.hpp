#ifndef ENTITY_LISTENER_HPP_INCLUDED
#define ENTITY_LISTENER_HPP_INCLUDED

#include "SharedTypes.hpp"

class EntityListener
{
public:
    virtual void onEntityCreate(int e) {}
    virtual void onEntityDestroy(int e) {}
    virtual void onEntityUpdate(int e, Time time) {}
};

#endif/*ENTITY_LISTENER_HPP_INCLUDED*/
