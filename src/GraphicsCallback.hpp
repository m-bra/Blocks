#ifndef GRAPHICS_CALLBACK_HPP_INCLUDED
#define GRAPHICS_CALLBACK_HPP_INCLUDED

#include "vec.hpp"

namespace blocks
{

class GraphicsCallback
{
public:
    virtual void setWindowSize(int w, int h) = 0;
    virtual void buildChunk(ivec3_c &c);
};

}

#endif//GRAPHICS_CALLBACK_HPP_INCLUDED
