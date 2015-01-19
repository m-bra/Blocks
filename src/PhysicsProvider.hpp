#ifndef PHYSICS_CALLBACK_HPP_INCLUDED
#define PHYSICS_CALLBACK_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning #include "precompiled.hpp" at the beginning of the TU!
#include "precompiled.hpp"
#endif

namespace blocks
{

class PhysicsProvider
{
public:
    virtual ~PhysicsProvider() {}

    virtual void setEntityPos(Entity e, fvec3_c &pos) = 0;
    virtual fvec3 getEntityPos(Entity e) = 0;
    virtual void getEntityOpenGLMatrix(Entity e, float *matrix) = 0;
    virtual void addEntityForce(Entity e, fvec3_c &) = 0;
    virtual void setEntityForce(Entity e, fvec3_c &) = 0;
    virtual void addEntityImpulse(Entity e, fvec3_c &) = 0;
    virtual void setEntityImpulse(Entity e, fvec3_c &) = 0;
    virtual void addEntityVelocity(Entity e, fvec3_c &) = 0;
    virtual void setEntityVelocity(Entity e, fvec3_c &) = 0;

    virtual bool getSelectedBlock(fvec3_c &from, fvec3_c &to, ivec3 &b1, ivec3 &b2) = 0;
    virtual int getSelectedEntity(fvec3_c &from, fvec3_c &to) = 0;
};

}

#endif//PHYSICS_CALLBACK_HPP_INCLUDED
