#ifndef CHUNK_LISTENER_HPP_INCLUDED
#define CHUNK_LISTENER_HPP_INCLUDED

#include "vec.hpp"

class ChunkListener
{
public:
    virtual bool canMove() {return true;}
    virtual void move(ivec3_c &m) {}
};

#endif/*CHUNK_LISTENER_HPP_INCLUDED*/
