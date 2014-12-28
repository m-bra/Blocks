#ifndef PHYSICS_CALLBACK_HPP_INCLUDED
#define PHYSICS_CALLBACK_HPP_INCLUDED

#include "vec.hpp"

namespace blocks
{

class PhysicsCallback
{
public:
    virtual void setEntityPos(int e, fvec3_c &pos) = 0;
    virtual fvec3 getEntityPos(int e) = 0;
    virtual void getEntityOpenGLMatrix(int e, float *matrix) = 0;
};

}

#endif//PHYSICS_CALLBACK_HPP_INCLUDED
