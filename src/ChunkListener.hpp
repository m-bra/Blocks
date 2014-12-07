#ifndef CHUNK_LISTENER_HPP_INCLUDED
#define CHUNK_LISTENER_HPP_INCLUDED

#include "vec.hpp"

namespace blocks
{

class ChunkListener
{
public:
    virtual bool canMove() = 0;
    virtual void move(ivec3_c &m) = 0;
    virtual void onChunkChange(ivec3_c &c) = 0;
};

}
#endif/*CHUNK_LISTENER_HPP_INCLUDED*/
